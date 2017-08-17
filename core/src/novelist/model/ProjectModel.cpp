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
#include "functional/Overloaded.h"
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

        connect(this, &ProjectModel::dataChanged, this, &ProjectModel::onDataChanged);
        connect(this, &ProjectModel::layoutChanged, this, &ProjectModel::onLayoutChanged);
        connect(this, &ProjectModel::rowsInserted, this, &ProjectModel::onRowsInserted);
        connect(this, &ProjectModel::rowsRemoved, this, &ProjectModel::onRowsRemoved);
    }

    ProjectProperties const& ProjectModel::properties() const
    {
        return std::get<ProjectHeadData>(m_root[0].m_data).m_properties;
    }

    void ProjectModel::setProperties(ProjectProperties const& properties)
    {
        std::get<ProjectHeadData>(m_root[0].m_data).m_properties = properties;
        m_modified = true;
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

        auto getDisplayText = Overloaded {
                [](auto const&) { return QString{""}; },
                [](ProjectHeadData const& arg) { return arg.m_properties.m_name; },
                [](NotebookHeadData const&) { return tr("Notebook"); },
                [](SceneData const& arg) { return arg.m_name; },
                [](ChapterData const& arg) { return arg.m_name; },
        };

        auto makeEmptyNote = [](QString const& s) {
            if (s.isEmpty())
                return tr("<unnamed>");
            return s;
        };

        QString displayText = std::visit(getDisplayText, item->m_data);
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
            default:
                return QVariant{};
        }
    }

    bool ProjectModel::setData(QModelIndex const& index, QVariant const& value, int role)
    {
        if (index.isValid() && role == Qt::EditRole) {

            auto* item = static_cast<Node*>(index.internalPointer());
            std::visit(Overloaded {
                    [](auto&) { },
                    [&value](ProjectHeadData& arg) { arg.m_properties.m_name = value.toString(); },
                    [&value](SceneData& arg) { arg.m_name = value.toString(); },
                    [&value](ChapterData& arg) { arg.m_name = value.toString(); },
            }, item->m_data);
            emit dataChanged(index, index);
            return true;
        }
        return false;
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
        if (count <= 0)
            return false;

        if (!parent.isValid())
            return false;

        auto* parentNode = static_cast<Node*>(parent.internalPointer());

        Expects(row >= 0);
        Expects(static_cast<size_t>(row + count - 1) <= parentNode->size());

        // Can't add children to scenes or the invisible root node
        if (!canInsert(parent))
            return false;

        QModelIndexList indicesThatChange = childIndices(parent);

        beginInsertRows(parent, row, row + count - 1);
        for (int r = 0; r < count; ++r)
            parentNode->emplace(parentNode->begin() + row + r, makeNodeData(type, name));
        endInsertRows();

        // All children of parent might have been relocated, therefore update their persistent indices
        QModelIndexList newIndices = childIndices(parent);
        newIndices.erase(newIndices.begin() + row, newIndices.begin() + row + count);
        Q_ASSERT(indicesThatChange.size() == newIndices.size());
        changePersistentIndexList(indicesThatChange, newIndices);

        return true;
    }

    bool ProjectModel::removeRows(int row, int count, QModelIndex const& parent)
    {
        if (!parent.isValid())
            return false;

        auto* item = static_cast<Node*>(parent.internalPointer());

        if (count <= 0)
            return false;

        Expects(row >= 0);
        Expects(count >= 0);
        Expects(item->size() > static_cast<size_t>(row));

        if (!canRemove(parent))
            return false;

        QModelIndexList indicesThatChange = childIndices(parent);

        beginRemoveRows(parent, row, row + count - 1);
        for (int r = 0; r < count; ++r) {
            auto idx = parent.child(row, 0);
            emit beforeItemRemoved(idx, nodeType(idx));
            item->erase(item->begin() + row);
        }
        endRemoveRows();

        // All children of parent might have been relocated, therefore update their persistent indices
        QModelIndexList newIndices = childIndices(parent);
        for (int i = 0; i < count; ++i)
            newIndices.insert(row, QModelIndex{});
        Q_ASSERT(indicesThatChange.size() == newIndices.size());
        changePersistentIndexList(indicesThatChange, newIndices);

        return true;
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

    SceneDocument* ProjectModel::loadScene(QModelIndex const& index)
    {
        Expects(nodeType(index) == NodeType::Scene);

        auto& scene = std::get<SceneData>(static_cast<Node*>(index.internalPointer())->m_data);
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

        auto& scene = std::get<SceneData>(static_cast<Node*>(index.internalPointer())->m_data);
        scene.m_doc = nullptr;
    }

    bool ProjectModel::isContentModified(QModelIndex const& index) const
    {
        if (nodeType(index) == NodeType::Scene) {
            auto const& scene = std::get<SceneData>(static_cast<Node*>(index.internalPointer())->m_data);
            if (scene.m_doc != nullptr)
                return scene.m_doc->isModified();
        }

        return false;
    }

    bool ProjectModel::isStructureModified() const
    {
        return m_modified;
    }

    bool ProjectModel::isModified() const
    {
        if (isStructureModified())
            return true;

        bool contentModified = false;
        traverse_dfs(m_root, [&](Node const& n) {
            auto idx = createIndex(n.parentIndex().value_or(0), 0, const_cast<Node*>(&n));
            return contentModified &= isContentModified(idx);
        });
        return contentModified;
    }

    bool ProjectModel::moveRows(QModelIndex const& sourceParent, int sourceRow, int count,
            QModelIndex const& destinationParent, int destinationChild)
    {
        Expects(count > 0);

        bool success = true;
        for (int i = 0; i < count; ++i)
            success &= moveRowInternal(sourceParent, sourceRow + count - i - 1, destinationParent, destinationChild);

        return success;
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
        if (success)
            m_modified = false;
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
                auto& data = std::get<SceneData>(n.m_data);
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

        if (success)
            m_modified = false;

        return success;
    }

    void ProjectModel::setSaveDir(QDir const& dir)
    {
        m_saveDir = dir;
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

    std::ostream& operator<<(std::ostream& stream, ProjectModel const& model)
    {
        stream << model.m_root;
        return stream;
    }

    void ProjectModel::createRootNodes(ProjectProperties const& properties)
    {
        m_root.emplace_back(ProjectHeadData{properties});
        m_root.emplace_back(NotebookHeadData{});
    }

    bool ProjectModel::readInternal(QXmlStreamReader& xml)
    {
        Q_ASSERT(xml.isStartElement() && xml.name() == "novel");

        while (xml.readNextStartElement()) {
            if (xml.name() == "meta") {
                ProjectProperties properties;
                if (xml.attributes().hasAttribute("name"))
                    properties.m_name = xml.attributes().value("name").toString();
                if (xml.attributes().hasAttribute("author"))
                    properties.m_author = xml.attributes().value("author").toString();
                if (xml.attributes().hasAttribute("lang"))
                    properties.m_lang = lang::fromIdentifier(xml.attributes().value("lang").toString());
                setProperties(properties);

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
            insertRow(idx, InsertableNodeType::Chapter, name, parent);
            auto* node = &static_cast<Node*>(parent.internalPointer())->at(idx);
            auto& chapter = std::get<ChapterData>(node->m_data);
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

            insertRow(idx, InsertableNodeType::Scene, name, parent);
            auto* node = &static_cast<Node*>(parent.internalPointer())->at(idx);
            auto& scene = std::get<SceneData>(node->m_data);
            if (scene.m_id.id() != id)
                scene.m_id = m_sceneIdMgr.request(id);

            xml.skipCurrentElement();
        }
        else {
            xml.skipCurrentElement();
        }

        return true;
    }

    bool
    ProjectModel::moveRowInternal(QModelIndex const& sourceParent, int sourceRow, QModelIndex const& destinationParent,
            int destinationRow)
    {
        Expects(sourceRow >= 0);
        Expects(sourceRow < rowCount(sourceParent));
        Expects(destinationRow >= 0);
        Expects(destinationRow <= rowCount(destinationParent));

        if (!sourceParent.isValid() || !destinationParent.isValid())
            return false;

        if (!canInsert(destinationParent) || !canRemove(sourceParent))
            return false;

        auto* srcParent = static_cast<Node*>(sourceParent.internalPointer());
        auto* destParent = static_cast<Node*>(destinationParent.internalPointer());

        if (sourceParent == destinationParent && destinationRow >= sourceRow && destinationRow < sourceRow)
            return false;

        if (destParent->inSubtreeOf(srcParent->at(sourceRow)) || destParent == &srcParent->at(sourceRow))
            return false;

        auto fixNodePtrs = [fixSrc = srcParent->parent() == destParent,
                fixDest = destParent->parent() == srcParent,
                srcParentIdx = srcParent->parentIndex(),
                destParentIdx = destParent->parentIndex(),
                &srcParent, &destParent, sourceRow, destinationRow]() {

            if (fixDest) {
                size_t destParIdx = destParentIdx.value();
                if (sourceRow < destParentIdx)
                    --destParIdx;
                destParent = &srcParent->at(destParIdx);
            }
            else if (fixSrc) {
                size_t srcParIdx = srcParentIdx.value();
                if (destinationRow <= srcParentIdx)
                    ++srcParIdx;
                srcParent = &destParent->at(srcParIdx);
            }
        };

        QPersistentModelIndex persistentSrcParent{sourceParent};
        QPersistentModelIndex persistentDestParent{destinationParent};
        QList<QPersistentModelIndex> layoutChangeParents = {persistentSrcParent, persistentDestParent};
        emit layoutAboutToBeChanged(layoutChangeParents);

        QModelIndexList indicesThatChangeAtSrc = childIndices(sourceParent);
        QModelIndexList indicesThatChangeAtDest = childIndices(destinationParent);

        srcParent->move(sourceRow, *destParent, destinationRow);

        // All children of parent might have been relocated, therefore update their persistent indices
        if (sourceParent == destinationParent) {
            QModelIndexList newIndicesAtSrc = childIndices(sourceParent);
            Q_ASSERT(indicesThatChangeAtSrc.size() == newIndicesAtSrc.size());
            if (destinationRow > sourceRow)
                --destinationRow;
            newIndicesAtSrc.move(destinationRow, sourceRow);
            changePersistentIndexList(indicesThatChangeAtSrc, newIndicesAtSrc);
        }
        else {
            fixNodePtrs();
            QModelIndex newDest = createIndex(destinationRow, 0, &destParent->at(destinationRow));
            QModelIndexList newIndicesAtSrc = childIndices(*srcParent);
            newIndicesAtSrc.insert(sourceRow, newDest);
            Q_ASSERT(indicesThatChangeAtSrc.size() == newIndicesAtSrc.size());
            changePersistentIndexList(indicesThatChangeAtSrc, newIndicesAtSrc);

            QModelIndexList newIndicesAtDest = childIndices(*destParent);
            newIndicesAtDest.erase(newIndicesAtDest.begin() + destinationRow,
                    newIndicesAtDest.begin() + destinationRow + 1);
            Q_ASSERT(indicesThatChangeAtDest.size() == newIndicesAtDest.size());
            changePersistentIndexList(indicesThatChangeAtDest, newIndicesAtDest);
        }

        emit layoutChanged(layoutChangeParents);

        return true;
    }

    void ProjectModel::writeChapterOrScene(QXmlStreamWriter& xml, QModelIndex item) const
    {
        auto const* node = static_cast<Node const*>(item.internalPointer());
        auto const& data = node->m_data;
        switch (nodeType(data)) {
            case NodeType::Chapter: {
                auto& chapterData = std::get<ChapterData>(data);
                xml.writeStartElement("chapter");
                xml.writeAttribute("name", chapterData.m_name);
                xml.writeAttribute("id", chapterData.m_id.toString().c_str());
                for (size_t r = 0; r < node->size(); ++r)
                    writeChapterOrScene(xml, index(r, 0, item));
                xml.writeEndElement();
                break;
            }
            case NodeType::Scene: {
                auto& sceneData = std::get<SceneData>(data);
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
        if (std::holds_alternative<InvisibleRootData>(nodeData))
            return NodeType::InvisibleRoot;
        else if (std::holds_alternative<ProjectHeadData>(nodeData))
            return NodeType::ProjectHead;
        else if (std::holds_alternative<NotebookHeadData>(nodeData))
            return NodeType::NotebookHead;
        else if (std::holds_alternative<SceneData>(nodeData))
            return NodeType::Scene;
        else if (std::holds_alternative<ChapterData>(nodeData))
            return NodeType::Chapter;

        if (nodeData.valueless_by_exception())
            throw std::runtime_error{"Variant is valueless by exception"};

        throw std::runtime_error{"Should never get here since variant must always hold a value or be valueless."};
    }

    ProjectModel::NodeData ProjectModel::makeNodeData(InsertableNodeType type, QString const& name)
    {
        switch (type) {
            case InsertableNodeType::Chapter:
                return ChapterData{name, m_chapterIdMgr.generate()};
            case InsertableNodeType::Scene:
                return SceneData {name, m_sceneIdMgr.generate(), nullptr};
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

    std::ostream& operator<<(std::ostream& stream, ProjectModel::NodeData const& nodeData)
    {
        using Type = ProjectModel::NodeType;

        switch (ProjectModel::nodeType(nodeData)) {
            case Type::InvisibleRoot:
                stream << "(root)";
                break;
            case Type::ProjectHead:
                stream << std::get<ProjectModel::ProjectHeadData>(nodeData).m_properties.m_name.toStdString();
                break;
            case Type::NotebookHead:
                stream << "Notebook";
                break;
            case Type::Chapter:
                stream << std::get<ProjectModel::ChapterData>(nodeData).m_name.toStdString();
                break;
            case Type::Scene:
                stream << std::get<ProjectModel::SceneData>(nodeData).m_name.toStdString();
                break;
        }

        return stream;
    }

    void
    ProjectModel::onDataChanged(QModelIndex const& /*topLeft*/, QModelIndex const& /*bottomRight*/,
            QVector<int> const& /*roles*/)
    {
        m_modified = true;
    }

    void ProjectModel::onLayoutChanged(QList<QPersistentModelIndex> const& /*parents*/,
            QAbstractItemModel::LayoutChangeHint /*hint*/)
    {
        m_modified = true;
    }

    void ProjectModel::onRowsInserted(QModelIndex const& /*parent*/, int /*first*/, int /*last*/)
    {
        m_modified = true;
    }

    void ProjectModel::onRowsRemoved(QModelIndex const& /*parent*/, int /*first*/, int /*last*/)
    {
        m_modified = true;
    }
}