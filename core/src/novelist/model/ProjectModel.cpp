/**********************************************************
 * @file   ProjectModel.cpp
 * @author jan
 * @date   24.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <gsl/gsl>
#include <QtCore/QTextStream>
#include <QDebug>
#include <QBrush>
#include <QIcon>
#include <stack>
#include "util/Overloaded.h"
#include "model/ProjectModel.h"

namespace novelist {
    ProjectModel::ProjectModel() noexcept
            :ProjectModel(ProjectProperties{})
    {
        // These types must be movable
        static_assert(std::is_move_constructible_v<InvisibleRootData> && std::is_move_assignable_v<InvisibleRootData>,
                "InvisibleRootData must be movable");
        static_assert(std::is_move_constructible_v<ProjectHeadData> && std::is_move_assignable_v<ProjectHeadData>,
                "ProjectHeadData must be movable");
        static_assert(std::is_move_constructible_v<NotebookHeadData> && std::is_move_assignable_v<NotebookHeadData>,
                "NotebookHeadData must be movable");
        static_assert(std::is_move_constructible_v<SceneData> && std::is_move_assignable_v<SceneData>,
                "SceneData must be movable");
        static_assert(std::is_move_constructible_v<ChapterData> && std::is_move_assignable_v<ChapterData>,
                "ChapterData must be movable");

        static_assert(std::is_move_constructible_v<Node> && std::is_move_assignable_v<Node>,
                "Node must be movable");
    }

    ProjectModel::ProjectModel(ProjectProperties const& properties, QObject* parent) noexcept
            :QAbstractItemModel(parent)
    {
        createRootNodes(properties);
    }

    ProjectProperties const& ProjectModel::properties() const
    {
        return std::get<ProjectHeadData>(*m_root[0].m_data).m_properties;
    }

    void ProjectModel::setProperties(ProjectProperties const& properties)
    {
        m_undoStack.push(new ModifyProjectPropertiesCommand(properties, this));
    }

    void ProjectModel::doSetProperties(ProjectProperties const& properties)
    {
        std::get<ProjectHeadData>(*m_root[0].m_data).m_properties = properties;
        emit dataChanged(projectRootIndex(), projectRootIndex());
    }

    QModelIndex ProjectModel::projectRootIndex() const
    {
        return index(0, 0, QModelIndex());
    }

    QModelIndex ProjectModel::notebookIndex() const
    {
        return index(1, 0, QModelIndex());
    }

    QModelIndex ProjectModel::index(int row, int column, QModelIndex const& parent) const
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex{};

        Node const* parentItem = &m_root;
        if (parent.isValid())
            parentItem = static_cast<Node*>(parent.internalPointer());

        Node const& childItem = parentItem->at(row);
        return createIndex(row, column, const_cast<Node*>(&childItem));
    }

    QModelIndex ProjectModel::parent(QModelIndex const& child) const
    {
        if (!child.isValid())
            return QModelIndex{};

        auto* childItem = static_cast<Node*>(child.internalPointer());
        Node const* parentItem = childItem->parent();

        if (parentItem == &m_root)
            return QModelIndex{};

        return createIndex(parentItem->parentIndex().value(), 0, const_cast<Node*>(parentItem));
    }

    int ProjectModel::rowCount(QModelIndex const& parent) const
    {
        Node const* parentItem = &m_root;
        if (parent.isValid())
            parentItem = static_cast<Node*>(parent.internalPointer());

        return gsl::narrow_cast<int>(parentItem->size());
    }

    int ProjectModel::columnCount(QModelIndex const& /*parent*/) const
    {
        return 1;
    }

    QVariant ProjectModel::data(QModelIndex const& index, int role) const
    {
        if (!index.isValid())
            return QVariant{};

        auto* item = static_cast<Node*>(index.internalPointer());

        auto makeEmptyNote = [](QString const& s) {
            if (s.isEmpty())
                return tr("<unnamed>");
            return s;
        };

        QString displayText = getDisplayText(*item);
        switch (role) {
            case Qt::DisplayRole:
                return makeEmptyNote(displayText);
            case Qt::ForegroundRole:
                if (isContentModified(index))
                    return QBrush(QColor(Qt::blue));
                if (displayText.isEmpty())
                    return QBrush(QColor(Qt::gray));
                return QBrush(QColor(Qt::black));
            case Qt::DecorationRole: {
                switch (nodeType(*item)) {
                    case NodeType::Scene:
                        return QIcon(":/icons/scene.png");
                    case NodeType::Chapter:
                        return QIcon(":/icons/chapter.png");
                    case NodeType::ProjectHead:
                        return QIcon(":/icons/book.png");
                    case NodeType::NotebookHead:
                        return QIcon(":/icons/notebook.png");
                    default:
                        return QVariant{};
                }
            }
            case Qt::EditRole: {
                return displayText;
            }
            case DocumentRole: {
                if (nodeType(*item) == NodeType::Scene) {
                    auto& scene = std::get<SceneData>(*item->m_data);
                    return QVariant::fromValue(scene.m_doc.get());
                }
                [[fallthrough]];
            }
            default:
                return QVariant{};
        }
    }

    bool ProjectModel::setData(QModelIndex const& index, QVariant const& value, int role)
    {
        if (index.isValid() && role == Qt::EditRole) {
            m_undoStack.push(new ModifyNameCommand(value.toString(), index, this));
            return true;
        }
        return false;
    }

    bool ProjectModel::doSetName(QModelIndex const& index, QString const& name)
    {
        auto* item = static_cast<Node*>(index.internalPointer());
        std::visit(Overloaded {
                [](auto&) { },
                [&name](ProjectHeadData& arg) { arg.m_properties.m_name = name; },
                [&name](SceneData& arg) {
                    arg.m_name = name;
                    arg.m_doc->setMetaInformation(QTextDocument::DocumentTitle, name);
                },
                [&name](ChapterData& arg) { arg.m_name = name; },
        }, *item->m_data);
        emit dataChanged(index, index);
        return true;
    }

    QVariant ProjectModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
    {
        return QVariant{};
    }

    bool ProjectModel::setHeaderData(int /*section*/, Qt::Orientation /*orientation*/, const QVariant& /*value*/,
            int /*role*/)
    {
        return false;
    }

    Qt::ItemFlags ProjectModel::flags(QModelIndex const& index) const
    {
        if (!index.isValid())
            return Qt::ItemIsEnabled;

        Qt::ItemFlags flags = QAbstractItemModel::flags(index);

        auto* item = static_cast<Node*>(index.internalPointer());
        Q_ASSERT(item != nullptr);
        switch (nodeType(*item)) {
            case NodeType::Scene:
                flags |= Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
                break;
            case NodeType::Chapter:
                flags |= Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
                break;
            case NodeType::ProjectHead:
                flags |= Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
                break;
            case NodeType::NotebookHead:
                flags |= Qt::ItemIsDropEnabled;
                break;
            default:
                break;
        }

        return flags;
    }

    Qt::DropActions ProjectModel::supportedDropActions() const
    {
        return Qt::MoveAction;
    }

    Qt::DropActions ProjectModel::supportedDragActions() const
    {
        return Qt::MoveAction;
    }

    bool ProjectModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int /*column*/,
            const QModelIndex& parent)
    {
        if (action != Qt::MoveAction)
            return false;

        if (!data->hasFormat("application/x-projectmodelnodeindexlist"))
            return false;

        QByteArray encodedData = data->data("application/x-projectmodelnodeindexlist");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        QModelIndexList newItems;

        while (!stream.atEnd()) {
            int size;
            stream >> size;
            QModelIndex text;
            stream.readRawData(reinterpret_cast<char*>(&text), size);
            newItems << text;
        }

        auto destParent = parent;
        if (row < 0)
            row = 0;

        for (auto const& idx : newItems)
            moveRow(idx.parent(), idx.row(), destParent, row);

        return true;
    }

    QMimeData* ProjectModel::mimeData(const QModelIndexList& indexes) const
    {
        auto* mimeData = new QMimeData();
        QByteArray encodedData;

        QDataStream stream(&encodedData, QIODevice::WriteOnly);

        for (QModelIndex const& index : indexes) {
            stream << static_cast<int>(sizeof(index));
            stream.writeRawData(reinterpret_cast<char const*>(&index), sizeof(index));
        }

        mimeData->setData("application/x-projectmodelnodeindexlist", encodedData);
        return mimeData;
    }

    QStringList ProjectModel::mimeTypes() const
    {
        return {"application/x-projectmodelnodeindexlist"};
    }

    bool ProjectModel::canInsert(QModelIndex const& parent) const
    {
        if (!parent.isValid())
            return false;

        auto* item = static_cast<Node*>(parent.internalPointer());
        return nodeType(*item) == NodeType::Chapter || nodeType(*item) == NodeType::ProjectHead
                || nodeType(*item) == NodeType::NotebookHead;
    }

    bool ProjectModel::canRemove(QModelIndex const& parent) const
    {
        if (!parent.isValid())
            return false;

        auto* item = static_cast<Node*>(parent.internalPointer());
        return nodeType(*item) != NodeType::InvisibleRoot;
    }

    bool ProjectModel::insertRow(int row, ProjectModel::InsertableNodeType type, QString const& name,
            QModelIndex const& parent)
    {
        return insertRows(row, 1, type, name, parent);
    }

    bool ProjectModel::insertRows(int row, int count, InsertableNodeType type, QString const& name,
            QModelIndex const& parent)
    {
        if (!parent.isValid())
            return false;
        if (!canInsert(parent))
            return false;
        if (row < 0)
            return false;
        if (count <= 0)
            return false;
        if (rowCount(parent) < row + count - 1)
            return false;

        for (int r = 0; r < count; ++r)
            m_undoStack.push(new InsertRowCommand(Node(makeNodeData(type, name)), parent, row + r, this));

        return true;
    }

    void ProjectModel::doInsertRow(ProjectModel::Node n, int row, QModelIndex const& parent)
    {
        auto* parentNode = static_cast<Node*>(parent.internalPointer());

        QModelIndexList indicesThatChange = childIndices(parent);

        beginInsertRows(parent, row, row);
        parentNode->insert(parentNode->begin() + row, std::move(n));
        endInsertRows();

        // All children of parent might have been relocated, therefore update their persistent indices
        QModelIndexList newIndices = childIndices(parent);
        newIndices.erase(newIndices.begin() + row, newIndices.begin() + row + 1);
        Q_ASSERT(indicesThatChange.size() == newIndices.size());
        changePersistentIndexList(indicesThatChange, newIndices);
    }

    bool ProjectModel::removeRows(int row, int count, QModelIndex const& parent)
    {
        if (!parent.isValid())
            return false;
        if (!canRemove(parent))
            return false;
        if (row < 0)
            return false;
        if (count <= 0)
            return false;
        if (rowCount(parent) <= row + count - 1)
            return false;

        for (int r = 0; r < count; ++r)
            m_undoStack.push(new RemoveRowCommand(parent, row, this));

        return true;
    }

    void ProjectModel::doRemoveRow(int row, QModelIndex const& parent)
    {
        doTakeRow(row, parent);
    }

    ProjectModel::Node ProjectModel::doTakeRow(int row, QModelIndex const& parent)
    {
        auto* item = static_cast<Node*>(parent.internalPointer());

        QModelIndexList indicesThatChange = childIndices(parent);

        beginRemoveRows(parent, row, row);
        auto idx = parent.child(row, 0);
        emit beforeItemRemoved(idx, nodeType(idx));
        Node takenNode = item->take(item->begin() + row);
        endRemoveRows();

        // All children of parent might have been relocated, therefore update their persistent indices
        QModelIndexList newIndices = childIndices(parent);
        newIndices.insert(row, QModelIndex{});
        Q_ASSERT(indicesThatChange.size() == newIndices.size());
        changePersistentIndexList(indicesThatChange, newIndices);

        return takenNode;
    }

    void ProjectModel::clear()
    {
        beginResetModel();
        removeRows(0, gsl::narrow_cast<int>(m_root.at(0).size()), projectRootIndex());
        removeRows(0, gsl::narrow_cast<int>(m_root.at(1).size()), notebookIndex());
        endResetModel();
    }

    ProjectModel::NodeType ProjectModel::nodeType(QModelIndex const& index) const
    {
        Expects(index.isValid());

        return nodeType(*static_cast<Node*>(index.internalPointer()));
    }

    ProjectModel::NodeData const& ProjectModel::nodeData(QModelIndex const& index) const
    {
        Expects(index.isValid());

        auto* node = static_cast<Node*>(index.internalPointer());
        return node->m_data;
    }

    QString ProjectModel::nodeName(QModelIndex const& index) const
    {
        auto* node = static_cast<Node*>(index.internalPointer());
        return getDisplayText(*node);
    }

    SceneDocument* ProjectModel::loadScene(QModelIndex const& index)
    {
        Expects(nodeType(index) == NodeType::Scene);

        auto& scene = std::get<SceneData>(*static_cast<Node*>(index.internalPointer())->m_data);
        QString filename = QString::fromStdString(scene.m_id.toString() + ".xml");
        scene.m_doc = std::make_unique<SceneDocument>();
        if (auto d = contentDir(); d.exists(filename)) {
            QFile file {d.path() + QString{"/"} + filename};
            scene.m_doc->read(file);
        }

        // Make sure the dataChanged() signal is fired every time the document's modified state is changed
        connect(scene.m_doc.get(), &SceneDocument::modificationChanged,
                [this, perIdx = QPersistentModelIndex{index}](bool /*changed*/) {
                    emit dataChanged(perIdx, perIdx, {Qt::ForegroundRole});
                });

        return scene.m_doc.get();
    }

    void ProjectModel::unloadScene(QModelIndex const& index)
    {
        Expects(nodeType(index) == NodeType::Scene);

        auto& scene = std::get<SceneData>(*static_cast<Node*>(index.internalPointer())->m_data);
        scene.m_doc = nullptr;
    }

    bool ProjectModel::isContentModified(QModelIndex const& index) const
    {
        if (nodeType(index) == NodeType::Scene) {
            auto const& scene = std::get<SceneData>(*static_cast<Node*>(index.internalPointer())->m_data);
            if (scene.m_doc != nullptr)
                return scene.m_doc->isModified();
        }

        return false;
    }

    bool ProjectModel::isStructureModified() const
    {
        return !m_undoStack.isClean();
    }

    bool ProjectModel::isModified() const
    {
        if (isStructureModified())
            return true;

        bool contentModified = false;
        traverse_dfs(m_root, [&](Node const& n) -> bool {
            auto idx = createIndex(n.parentIndex().value_or(0), 0, const_cast<Node*>(&n));
            contentModified = contentModified || isContentModified(idx);
            return contentModified;
        });
        return contentModified;
    }

    bool ProjectModel::moveRows(QModelIndex const& sourceParent, int sourceRow, int count,
            QModelIndex const& destinationParent, int destinationChild)
    {
        if (!sourceParent.isValid() || !destinationParent.isValid())
            return false;
        if (!canRemove(sourceParent) || !canInsert(destinationParent))
            return false;
        if (sourceRow < 0 || destinationChild < 0)
            return false;
        if (count <= 0)
            return false;
        if (rowCount(sourceParent) <= sourceRow + count - 1)
            return false;

        auto* srcParent = static_cast<Node*>(sourceParent.internalPointer());
        auto* destParent = static_cast<Node*>(destinationParent.internalPointer());

        if (sourceParent == destinationParent && destinationChild >= sourceRow && destinationChild < sourceRow)
            return false;
        if (destParent->inSubtreeOf(srcParent->at(sourceRow)) || destParent == &srcParent->at(sourceRow))
            return false;

        for (int r = 0; r < count; ++r)
            m_undoStack.push(
                    new MoveRowCommand(sourceParent, sourceRow + count - r - 1, destinationParent, destinationChild,
                            this));

        return true;
    }

    bool ProjectModel::read(QFile& file)
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString xml = file.readAll();
        file.close();

        return read(xml);
    }

    bool ProjectModel::read(QString const& xml)
    {
        // Clear previous data
        clear();
        m_undoStack.clear();

        QXmlStreamReader xmlReader(xml);
        if (xmlReader.readNextStartElement()) {
            if (xmlReader.name() == "novel" && xmlReader.attributes().value("version") == "1.0")
                if (readInternal(xmlReader))
                    return true;
        }
        if (xmlReader.hasError())
            qWarning() << "Error while reading project file." << xmlReader.errorString() << "At line"
                       << xmlReader.lineNumber() << ", column" << xmlReader.columnNumber() << ", character offset"
                       << xmlReader.characterOffset();

        return false;
    }

    bool ProjectModel::write(QFile& file) const
    {
        QString xml;
        if (!write(xml))
            return false;

        if (!file.open(QIODevice::WriteOnly))
            return false;

        QTextStream stream(&file);
        stream << xml;

        file.close();

        return true;
    }

    bool ProjectModel::write(QString& xml) const
    {
        // Write project xml file
        //
        QXmlStreamWriter xmlWriter(&xml);
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();
        xmlWriter.writeDTD("<!DOCTYPE novel>");
        xmlWriter.writeStartElement("novel");
        xmlWriter.writeAttribute("version", "1.0");

        // Write project root information
        xmlWriter.writeStartElement("meta");
        xmlWriter.writeAttribute("name", properties().m_name);
        xmlWriter.writeAttribute("author", properties().m_author);
        xmlWriter.writeAttribute("lang", lang::identifier(properties().m_lang));
        xmlWriter.writeEndElement();

        // Write project chapters and scenes
        xmlWriter.writeStartElement("content");
        for (int r = 0; r < rowCount(projectRootIndex()); ++r)
            writeChapterOrScene(xmlWriter, projectRootIndex().child(r, 0));
        xmlWriter.writeEndElement();

        // Write notebook
        xmlWriter.writeStartElement("notebook");
        for (int r = 0; r < rowCount(notebookIndex()); ++r)
            writeChapterOrScene(xmlWriter, notebookIndex().child(r, 0));
        xmlWriter.writeEndElement();

        xmlWriter.writeEndDocument();

        return true;
    }

    bool ProjectModel::open(QDir const& dir)
    {
        m_saveDir = dir;
        QFile file{dir.path() + "/project.xml"};
        bool success = read(file);
        if (success) {
            m_undoStack.clear();
            m_neverSaved = false;
        }
        return success;
    }

    bool ProjectModel::save()
    {
        QString contentPath = contentDir().path() + "/";
        QFile file{m_saveDir.path() + "/project.xml"};

        if (!m_saveDir.exists())
            return false;
        else {
            if (!m_saveDir.exists(m_contentDirName))
                m_saveDir.mkdir(m_contentDirName);
        }

        if (!write(file))
            return false;

        bool success = true;
        traverse_dfs(m_root, [&](Node& n) {
            if (nodeType(n) == NodeType::Scene) {
                auto& data = std::get<SceneData>(*n.m_data);
                if (data.m_doc != nullptr && data.m_doc->isModified()) {
                    QFile sceneFile{contentPath + QString::fromStdString(data.m_id.toString() + ".xml")};
                    bool localSuccess = data.m_doc->write(sceneFile);
                    if (localSuccess)
                        data.m_doc->setModified(false);
                    success &= localSuccess;
                }
            }
            return false;
        });

        if (success) {
            m_undoStack.setClean();
            m_neverSaved = false;
        }

        return success;
    }

    void ProjectModel::setSaveDir(QDir const& dir)
    {
        m_saveDir = dir;
        m_neverSaved = true;
    }

    QDir const& ProjectModel::saveDir() const
    {
        return m_saveDir;
    }

    QDir ProjectModel::contentDir() const
    {
        QDir contentDir = saveDir();
        contentDir.cd(m_contentDirName);
        return contentDir;
    }

    bool ProjectModel::neverSaved() const
    {
        return m_neverSaved;
    }

    QUndoStack& ProjectModel::undoStack() noexcept
    {
        return m_undoStack;
    }

    std::ostream& operator<<(std::ostream& stream, ProjectModel const& model)
    {
        stream << model.m_root;
        return stream;
    }

    void ProjectModel::createRootNodes(ProjectProperties const& properties)
    {
        m_root.emplace_back(std::make_shared<NodeDataUnique>(ProjectHeadData{properties}));
        m_root.emplace_back(std::make_shared<NodeDataUnique>(NotebookHeadData{}));
    }

    bool ProjectModel::readInternal(QXmlStreamReader& xml)
    {
        Q_ASSERT(xml.isStartElement() && xml.name() == "novel");

        while (xml.readNextStartElement()) {
            if (xml.name() == "meta") {
                ProjectProperties properties{};
                if (xml.attributes().hasAttribute("name"))
                    properties.m_name = xml.attributes().value("name").toString();
                if (xml.attributes().hasAttribute("author"))
                    properties.m_author = xml.attributes().value("author").toString();
                if (xml.attributes().hasAttribute("lang"))
                    properties.m_lang = lang::fromIdentifier(xml.attributes().value("lang").toString());
                doSetProperties(properties);

                xml.skipCurrentElement();
            }
            else if (xml.name() == "content") {
                while (xml.readNextStartElement()) {
                    if (!readChapterOrScene(xml, projectRootIndex()))
                        return false;
                }
            }
            else if (xml.name() == "notebook") {
                while (xml.readNextStartElement()) {
                    if (!readChapterOrScene(xml, notebookIndex()))
                        return false;
                }
            }
            else
                xml.skipCurrentElement();
        }
        return true;
    }

    bool ProjectModel::readChapterOrScene(QXmlStreamReader& xml, QModelIndex parent)
    {
        Q_ASSERT(xml.isStartElement() && (xml.name() == "chapter" || xml.name() == "scene"));
        Q_ASSERT(parent.isValid());

        int idx = rowCount(parent);

        if (xml.name() == "chapter") {
            QString name;
            uint32_t id{};
            if (xml.attributes().hasAttribute("name"))
                name = xml.attributes().value("name").toString();
            if (xml.attributes().hasAttribute("id"))
                id = static_cast<uint32_t>(xml.attributes().value("id").toULongLong());
            doInsertRow(Node(makeNodeData(InsertableNodeType::Chapter, name)), idx, parent);
            auto* node = &static_cast<Node*>(parent.internalPointer())->at(idx);
            auto& chapter = std::get<ChapterData>(*node->m_data);
            if (chapter.m_id.id() != id)
                chapter.m_id = m_chapterIdMgr.request(id);
            while (xml.readNextStartElement()) {
                if (!readChapterOrScene(xml, index(idx, 0, parent)))
                    return false;
            }
        }
        else if (xml.name() == "scene") {
            QString name;
            uint32_t id{};
            if (xml.attributes().hasAttribute("name"))
                name = xml.attributes().value("name").toString();
            if (xml.attributes().hasAttribute("id"))
                id = static_cast<uint32_t>(xml.attributes().value("id").toULongLong());

            doInsertRow(Node(makeNodeData(InsertableNodeType::Scene, name)), idx, parent);
            auto* node = &static_cast<Node*>(parent.internalPointer())->at(idx);
            auto& scene = std::get<SceneData>(*node->m_data);
            if (scene.m_id.id() != id)
                scene.m_id = m_sceneIdMgr.request(id);
            loadScene(parent.child(idx, 0));

            xml.skipCurrentElement();
        }
        else {
            xml.skipCurrentElement();
        }

        return true;
    }

    int
    ProjectModel::doMoveRow(QModelIndex const& sourceParent, int sourceRow, QModelIndex const& destinationParent,
            int destinationRow)
    {
        enum class MoveType { SameLevel, OneUp, OneDown, Other };
        MoveType const moveType = [&] () -> MoveType {
            if (sourceParent == destinationParent)
                return MoveType::SameLevel; // Both pointers stay valid
            else if(sourceParent.parent() == destinationParent)
                return MoveType::OneUp; // Source parent becomes invalid
            else if(destinationParent.parent() == sourceParent)
                return MoveType::OneDown; // Destination parent becomes invalid
            return MoveType::Other;
        } ();

        auto* srcParent = static_cast<Node*>(sourceParent.internalPointer());
        auto* destParent = static_cast<Node*>(destinationParent.internalPointer());

        QPersistentModelIndex persistentSrcParent{sourceParent};
        QPersistentModelIndex persistentDestParent{destinationParent};
        QList<QPersistentModelIndex> layoutChangeParents = {persistentSrcParent, persistentDestParent};
        emit layoutAboutToBeChanged(layoutChangeParents);

        QModelIndexList indicesThatChangeAtSrc = childIndices(sourceParent);
        QModelIndexList indicesThatChangeAtDest = childIndices(destinationParent);

        auto destIter = srcParent->move(sourceRow, *destParent, destinationRow);

        // All children of parent might have been relocated, therefore update their persistent indices
        switch(moveType) {
            case MoveType::SameLevel: {
                QModelIndexList newIndicesAtSrc = childIndices(sourceParent);
                Q_ASSERT(indicesThatChangeAtSrc.size() == newIndicesAtSrc.size());
                if (destinationRow > sourceRow)
                    --destinationRow;
                newIndicesAtSrc.move(destinationRow, sourceRow);
                changePersistentIndexList(indicesThatChangeAtSrc, newIndicesAtSrc);
            } break;
            case MoveType::Other:
                [[fallthrough]]
            case MoveType::OneUp: {
                QModelIndexList newIndicesAtDest = childIndices(destinationParent);
                auto moveToIdx = newIndicesAtDest.takeAt(std::distance(destParent->begin(), destIter));
                Q_ASSERT(indicesThatChangeAtDest.size() == newIndicesAtDest.size());
                changePersistentIndexList(indicesThatChangeAtDest, newIndicesAtDest);

                QModelIndexList newIndicesAtSrc = childIndices(persistentSrcParent);
                newIndicesAtSrc.insert(sourceRow, moveToIdx);
                Q_ASSERT(indicesThatChangeAtSrc.size() == newIndicesAtSrc.size());
                changePersistentIndexList(indicesThatChangeAtSrc, newIndicesAtSrc);
            } break;
            case MoveType::OneDown: {
                QModelIndexList newIndicesAtSrc = childIndices(sourceParent);
                auto oldSrcIdx = indicesThatChangeAtSrc.takeAt(sourceRow);
                Q_ASSERT(indicesThatChangeAtSrc.size() == newIndicesAtSrc.size());
                changePersistentIndexList(indicesThatChangeAtSrc, newIndicesAtSrc);

                destParent = static_cast<Node*>(persistentDestParent.internalPointer());
                QModelIndexList newIndicesAtDest = childIndices(persistentDestParent);
                indicesThatChangeAtDest.insert(std::distance(destParent->begin(), destIter), oldSrcIdx);
                Q_ASSERT(indicesThatChangeAtDest.size() == newIndicesAtDest.size());
                changePersistentIndexList(indicesThatChangeAtDest, newIndicesAtDest);
            } break;
        }

        emit layoutChanged(layoutChangeParents);

        return destinationRow;
    }

    void ProjectModel::writeChapterOrScene(QXmlStreamWriter& xml, QModelIndex item) const
    {
        auto const* node = static_cast<Node const*>(item.internalPointer());
        auto const& data = node->m_data;
        switch (nodeType(data)) {
            case NodeType::Chapter: {
                auto& chapterData = std::get<ChapterData>(*data);
                xml.writeStartElement("chapter");
                xml.writeAttribute("name", chapterData.m_name);
                xml.writeAttribute("id", chapterData.m_id.toString().c_str());
                for (size_t r = 0; r < node->size(); ++r)
                    writeChapterOrScene(xml, index(r, 0, item));
                xml.writeEndElement();
                break;
            }
            case NodeType::Scene: {
                auto& sceneData = std::get<SceneData>(*data);
                xml.writeStartElement("scene");
                xml.writeAttribute("name", sceneData.m_name);
                xml.writeAttribute("id", sceneData.m_id.toString().c_str());
                xml.writeEndElement();
                break;
            }
            default:
                break;
        }
    }

    ProjectModel::NodeType ProjectModel::nodeType(Node const& n)
    {
        return nodeType(n.m_data);
    }

    ProjectModel::NodeType ProjectModel::nodeType(NodeData const& nodeData)
    {
        if (std::holds_alternative<InvisibleRootData>(*nodeData))
            return NodeType::InvisibleRoot;
        else if (std::holds_alternative<ProjectHeadData>(*nodeData))
            return NodeType::ProjectHead;
        else if (std::holds_alternative<NotebookHeadData>(*nodeData))
            return NodeType::NotebookHead;
        else if (std::holds_alternative<SceneData>(*nodeData))
            return NodeType::Scene;
        else if (std::holds_alternative<ChapterData>(*nodeData))
            return NodeType::Chapter;

        if (nodeData->valueless_by_exception())
            throw std::runtime_error{"Variant is valueless by exception"};

        throw std::runtime_error{"Should never get here since variant must always hold a value or be valueless."};
    }

    QString ProjectModel::nodeTypeToString(InsertableNodeType type) const noexcept
    {
        switch (type) {
            case InsertableNodeType::Scene:
                return tr("scene");
            case InsertableNodeType::Chapter:
                return tr("chapter");
        }
        return "";
    }

    ProjectModel::NodeData ProjectModel::makeNodeData(InsertableNodeType type, QString const& name)
    {
        switch (type) {
            case InsertableNodeType::Chapter:
                return std::make_shared<NodeDataUnique>(ChapterData{name, m_chapterIdMgr.generate()});
            case InsertableNodeType::Scene:
                return std::make_shared<NodeDataUnique>(SceneData{name, m_sceneIdMgr.generate(),
                                                                  std::make_unique<SceneDocument>()});
        }

        throw std::runtime_error{"Should never get here. Probably forgot to update switch statement."};
    }

    QModelIndexList ProjectModel::childIndices(QModelIndex const& parent) const
    {
        auto const* node = static_cast<Node const*>(parent.internalPointer());
        QModelIndexList indices;
        for (size_t i = 0; i < node->size(); ++i)
            indices.append(index(i, 0, parent));

        return indices;
    }

    QModelIndexList ProjectModel::childIndices(Node const& n) const
    {
        QModelIndexList indices;
        for (size_t i = 0; i < n.size(); ++i)
            indices.append(createIndex(i, 0, const_cast<Node*>(&n.at(i))));

        return indices;
    }

    QString ProjectModel::getDisplayText(ProjectModel::Node const& n) const
    {
        auto displayTextFun = Overloaded {
                [](auto const&) { return QString{""}; },
                [](ProjectHeadData const& arg) { return arg.m_properties.m_name; },
                [](NotebookHeadData const&) { return tr("Notebook"); },
                [](SceneData const& arg) { return arg.m_name; },
                [](ChapterData const& arg) { return arg.m_name; },
        };

        QString displayText = std::visit(displayTextFun, *n.m_data);
        return displayText;
    }

    bool ProjectModel::operator==(ProjectModel const& other) const noexcept
    {
        std::stack<std::pair<Node const*, Node const*>> stack;
        stack.emplace(std::make_pair<Node const*, Node const*>(&m_root, &other.m_root));
        while (!stack.empty()) {
            auto nodes = stack.top();
            stack.pop();

            if (nodes.first->size() != nodes.second->size())
                return false;
            if (*nodes.first->m_data.get() != *nodes.second->m_data.get())
                return false;

            for (size_t i = 0; i < nodes.first->size(); ++i)
                stack.emplace(std::make_pair<Node const*, Node const*>(&nodes.first->at(i), &nodes.second->at(i)));
        }

        return true;
    }

    bool ProjectModel::operator!=(ProjectModel const& other) const noexcept
    {
        return !(*this == other);
    }

    std::ostream& operator<<(std::ostream& stream, ProjectModel::NodeData const& nodeData)
    {
        using Type = ProjectModel::NodeType;

        switch (ProjectModel::nodeType(nodeData)) {
            case Type::InvisibleRoot:
                stream << "(root)";
                break;
            case Type::ProjectHead:
                stream << std::get<ProjectModel::ProjectHeadData>(*nodeData).m_properties.m_name.toStdString();
                break;
            case Type::NotebookHead:
                stream << "Notebook";
                break;
            case Type::Chapter:
                stream << std::get<ProjectModel::ChapterData>(*nodeData).m_name.toStdString();
                break;
            case Type::Scene:
                stream << std::get<ProjectModel::SceneData>(*nodeData).m_name.toStdString();
                break;
        }

        return stream;
    }

    InsertRowCommand::InsertRowCommand(Node node, QModelIndex const& parentIdx, int row, ProjectModel* model,
            QUndoCommand* parent)
            :ProjectModelCommand(parent),
             m_node(std::move(node)),
             m_path(parentIdx),
             m_row(row),
             m_model(model)
    {
        setText(ProjectModel::tr("inserting \"%1\"").arg(model->getDisplayText(m_node)));
    }

    void InsertRowCommand::undo()
    {
        m_model->doRemoveRow(m_row, m_path.toModelIndex(m_model));
    }

    void InsertRowCommand::redo()
    {
        m_model->doInsertRow(m_node.clone(), m_row, m_path.toModelIndex(m_model));
    }

    RemoveRowCommand::RemoveRowCommand(QModelIndex const& parentIdx, int row, ProjectModel* model, QUndoCommand* parent)
            :ProjectModelCommand(parent),
             m_node(static_cast<Node*>(parentIdx.internalPointer())->at(row).clone()),
             m_path(parentIdx),
             m_row(row),
             m_model(model)
    {
        setText(ProjectModel::tr("removing \"%1\"")
                .arg(m_path.toModelIndex(m_model).child(m_row, 0).data().toString()));
    }

    void RemoveRowCommand::undo()
    {
        m_model->doInsertRow(m_node.clone(), m_row, m_path.toModelIndex(m_model));
    }

    void RemoveRowCommand::redo()
    {
        m_model->doRemoveRow(m_row, m_path.toModelIndex(m_model));
    }

    MoveRowCommand::MoveRowCommand(QModelIndex const& srcParent, int srcRow, QModelIndex const& destParent, int destRow,
            ProjectModel* model, QUndoCommand* parent)
            :ProjectModelCommand(parent),
             m_srcPath(srcParent),
             m_destPath(destParent),
             m_srcRow(srcRow),
             m_destRow(destRow),
             m_model(model)
    {
        setText(ProjectModel::tr("moving \"%1\"")
                .arg(m_srcPath.toModelIndex(m_model).child(m_srcRow, 0).data().toString()));
    }

    void MoveRowCommand::undo()
    {
        auto destPath = m_srcPath;
        destPath.emplace_back(m_srcRow);
        auto idx = m_afterMovePath.compare(destPath);
        if(idx < destPath.depth() && idx >= m_afterMovePath.depth())
            idx--;
        if(idx < destPath.depth() && idx < m_afterMovePath.depth() && idx > m_afterMovePath.depth() - 2
                && destPath[idx].first >= m_afterMovePath[idx].first)
            destPath[idx].first++;

        m_model->doMoveRow(m_afterMovePath.parentPath().toModelIndex(m_model), m_afterMovePath.leaf().first,
                destPath.parentPath().toModelIndex(m_model), destPath.leaf().first);
    }

    void MoveRowCommand::redo()
    {
        QModelIndex parSrcIdx(m_srcPath.toModelIndex(m_model));
        QPersistentModelIndex srcIdx = parSrcIdx.child(m_srcRow, 0);
        m_model->doMoveRow(parSrcIdx, m_srcRow, m_destPath.toModelIndex(m_model), m_destRow);
        m_afterMovePath = ModelPath(srcIdx);
    }

    ModifyNameCommand::ModifyNameCommand(QString const& name, QModelIndex const& idx, ProjectModel* model,
            QUndoCommand* parent)
            :ProjectModelCommand(parent),
             m_path(idx),
             m_model(model),
             m_name(name)
    {
        setText(ProjectModel::tr("changing name from \"%1\" to \"%2\"")
                .arg(m_path.toModelIndex(m_model).data().toString()).arg(m_name));
    }

    void ModifyNameCommand::undo()
    {
        QModelIndex idx = m_path.toModelIndex(m_model);
        QString redoData = idx.data().toString();
        m_model->doSetName(idx, m_name);
        m_name = redoData;
    }

    void ModifyNameCommand::redo()
    {
        QModelIndex idx = m_path.toModelIndex(m_model);
        QString undoData = idx.data().toString();
        m_model->doSetName(idx, m_name);
        m_name = undoData;
    }

    ModifyProjectPropertiesCommand::ModifyProjectPropertiesCommand(ProjectProperties props, ProjectModel* model,
            QUndoCommand* parent)
            :ProjectModelCommand(parent),
             m_properties(std::move(props)),
             m_model(model)
    {
        setText(ProjectModel::tr("changing project properties"));
    }

    void ModifyProjectPropertiesCommand::undo()
    {
        auto redoProperties = m_model->properties();
        m_model->doSetProperties(m_properties);
        m_properties = redoProperties;
    }

    void ModifyProjectPropertiesCommand::redo()
    {
        auto undoProperties = m_model->properties();
        m_model->doSetProperties(m_properties);
        m_properties = undoProperties;
    }
}