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
#include "functional/Overloaded.h"
#include "model/ProjectModel.h"

namespace novelist {
    ProjectModel::ProjectModel() noexcept
            :ProjectModel(ProjectProperties{})
    {
        // These types must be movable such that they
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
        return std::get<ProjectHeadData>(m_root[0].m_data).m_properties;
    }

    void ProjectModel::setProperties(ProjectProperties const& properties)
    {
        std::get<ProjectHeadData>(m_root[0].m_data).m_properties = properties;
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

        return createIndex(computeParentIndex(*parentItem), 0, const_cast<Node*>(parentItem));
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

        if (role == Qt::DisplayRole)
            return std::visit(Overloaded {
                    [](auto const&) { return QVariant{}; },
                    [](ProjectHeadData const& arg) { return QVariant{arg.m_properties.m_name}; },
                    [](NotebookHeadData const&) { return QVariant{tr("Notebook")}; },
                    [](SceneData const& arg) { return QVariant{arg.m_name}; },
                    [](ChapterData const& arg) { return QVariant{arg.m_name}; },
            }, item->m_data);

        return QVariant{};
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

    Qt::ItemFlags ProjectModel::flags(QModelIndex const& index) const
    {
        if (!index.isValid())
            return Qt::ItemIsEnabled;

        Qt::ItemFlags flags = QAbstractItemModel::flags(index);

        auto* item = static_cast<Node*>(index.internalPointer());
        if (nodeType(*item) == NodeType::Scene || nodeType(*item) == NodeType::Chapter)
            flags |= Qt::ItemIsEditable;

        return flags;
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

    bool ProjectModel::insertRow(int row, ProjectModel::InsertableNodeType type, QString const& name, QModelIndex const& parent)
    {
        return insertRows(row, 1, type, name, parent);
    }

    bool ProjectModel::insertRows(int row, int count, InsertableNodeType type, QString const& name, QModelIndex const& parent)
    {
        auto* item = static_cast<Node*>(parent.internalPointer());

        Expects(row >= 0);
        Expects(static_cast<size_t>(row) <= item->size());

        // Can't add children to scenes or the invisible root node
        if (!canInsert(parent))
            return false;

        beginInsertRows(parent, row, row + count - 1);
        for (int r = 0; r < count; ++r)
            item->emplace(item->begin() + row, makeNodeData(type, name));

        endInsertRows();
        return true;
    }

    bool ProjectModel::removeRows(int row, int count, QModelIndex const& parent)
    {
        auto* item = static_cast<Node*>(parent.internalPointer());

        if (count <= 0)
            return false;

        Expects(row >= 0);
        Expects(count >= 0);
        Expects(item->size() > static_cast<size_t>(row));

        if (!canRemove(parent))
            return false;

        beginRemoveRows(QModelIndex(), row, row + count - 1);

        for (int r = 0; r < count; ++r)
            item->erase(item->begin() + row);

        endRemoveRows();
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

    bool ProjectModel::moveRows(QModelIndex const& sourceParent, int sourceRow, int count,
            QModelIndex const& destinationParent, int destinationChild)
    {
        Q_ASSERT(sourceRow >= 0);
        Q_ASSERT(destinationChild >= 0);
        Q_ASSERT(count > 0);

        if (!sourceParent.isValid() || !destinationParent.isValid())
            return false;

        if (!canInsert(destinationParent) || !canRemove(sourceParent))
            return false;

        auto* srcParent = static_cast<Node*>(sourceParent.internalPointer());
        auto* destParent = static_cast<Node*>(destinationParent.internalPointer());

        if (sourceParent == destinationParent && destinationChild >= sourceRow && destinationChild <= sourceRow + count)
            return false;

        for (int i = 0; i < count; ++i) {
            if (destParent->inSubtreeOf(srcParent->at(sourceRow + i)) || destParent == &srcParent->at(sourceRow + i))
                return false;
        }

        beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

        for (int i = 0; i < count; ++i)
            srcParent->move(sourceRow, *destParent, destinationChild + i);

        endMoveRows();

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

        return false;
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
        xmlWriter.writeAttribute("lang", properties().m_lang.shortname());
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
                    properties.m_lang = Language(xml.attributes().value("lang").toString());
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
            if (xml.attributes().hasAttribute("name"))
                name = xml.attributes().value("name").toString();
            insertRow(idx, InsertableNodeType::Chapter, name, parent);
            while (xml.readNextStartElement()) {
                if (!readChapterOrScene(xml, index(idx, 0, parent)))
                    return false;
            }
        }
        else if (xml.name() == "scene") {
            QString name;
            uint32_t id;
            if (xml.attributes().hasAttribute("name"))
                name = xml.attributes().value("name").toString();
            if (xml.attributes().hasAttribute("uid"))
                id = xml.attributes().value("uid").toULongLong();

            insertRow(idx, InsertableNodeType::Scene, name, parent);
            auto* node = &static_cast<Node*>(parent.internalPointer())->at(idx);
            auto& scene = std::get<SceneData>(node->m_data);
            scene.m_id = m_sceneIdMgr.request(id);

            xml.skipCurrentElement();
        }
        else {
            xml.skipCurrentElement();
        }

        return true;
    }

    void ProjectModel::writeChapterOrScene(QXmlStreamWriter& xml, QModelIndex item) const
    {
        auto const* node = static_cast<Node const*>(item.internalPointer());
        auto const& data = node->m_data;
        switch (nodeType(data)) {
            case NodeType::Chapter: {
                xml.writeStartElement("chapter");
                xml.writeAttribute("name", std::get<ChapterData>(data).m_name);
                for (size_t r = 0; r < node->size(); ++r)
                    writeChapterOrScene(xml, index(r, 0, item));
                xml.writeEndElement();
                break;
            }
            case NodeType::Scene: {
                xml.writeStartElement("scene");
                xml.writeAttribute("name", std::get<SceneData>(data).m_name);
                xml.writeAttribute("uid", std::get<SceneData>(data).m_id.toString().c_str());
                xml.writeEndElement();
                break;
            }
            default:
                break;
        }
    }

    int ProjectModel::computeParentIndex(Node const& n) const
    {
        if (n.parent()) {
            auto iter = std::find(n.parent()->begin(), n.parent()->end(), n);
            if (iter != n.parent()->end())
                return gsl::narrow_cast<int>(std::distance(n.parent()->begin(), iter));
        }
        return -1;
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

        throw std::runtime_error{"Should never get here. Probably forgot to update switch statement."};
    }

    ProjectModel::NodeData ProjectModel::makeNodeData(InsertableNodeType type, QString const& name)
    {
        switch (type) {
            case InsertableNodeType::Chapter:
                return ChapterData{name, m_chapterIdMgr.generate()};
            case InsertableNodeType::Scene:
                return SceneData {name, m_sceneIdMgr.generate()};
        }

        throw std::runtime_error{"Should never get here. Probably forgot to update switch statement."};
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
}