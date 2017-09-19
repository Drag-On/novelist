/**********************************************************
 * @file   ProjectModel.h
 * @author jan
 * @date   24.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_PROJECTMODEL_H
#define NOVELIST_PROJECTMODEL_H

#include <memory>
#include <variant>
#include <QtCore/QVariant>
#include <QtCore/QList>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QXmlStreamReader>
#include <QtGui/QTextDocument>
#include <QtWidgets/QUndoStack>
#include <QMimeData>
#include "datastructures/SceneDocument.h"
#include "datastructures/Tree.h"
#include "identity/Identity.h"
#include "lang/Language.h"
#include "ModelPath.h"

namespace novelist {

    class InsertRowCommand;

    /**
     * Basic project properties
     */
    struct ProjectProperties {
        QString m_name; //!< project name
        QString m_author; //!< project author
        Language m_lang = Language::en_US; //!< project language
    };

    /**
     * The project model provides means to add chapters and scenes in a tree
     */
    class ProjectModel : public QAbstractItemModel {
    Q_OBJECT

    public:
        /**
         * All node types, including those that can not be added such as the root node
         */
        enum class NodeType {
            InvisibleRoot = 0, //!< The invisible root node
            ProjectHead, //!< Project head node
            NotebookHead, //!< Notebook head node
            Scene, //!< Scene
            Chapter, //!< Chapter
        };

        /**
         * Types of nodes which can be added
         */
        enum class InsertableNodeType {
            Scene, //!< Scene
            Chapter, //!< Chapter
        };

    private:
        struct Chapter_Tag {
        };
        struct Scene_Tag {
        };
    public:
        /**
         * Unique ID among all chapters
         */
        using ChapterId = IdManager<Chapter_Tag>::IdType;
        /**
         * Unique ID among all scenes
         */
        using SceneId = IdManager<Scene_Tag>::IdType;

        /**
         * Data of the invisible root node
         */
        struct InvisibleRootData {
        };

        /**
         * Data of the project root node
         */
        struct ProjectHeadData {
            ProjectProperties m_properties; //!< Project properties
        };

        /**
         * Data of the notebook root node
         */
        struct NotebookHeadData {
        };

        /**
         * Data of a scene node
         */
        struct SceneData {
            QString m_name;                       //!< Scene name
            SceneId m_id;                         //!< Scene ID
            std::unique_ptr<SceneDocument> m_doc; //!< Actual content if currently loaded
        };

        /**
         * Data of a chapter node
         */
        struct ChapterData {
            QString m_name; //!< Chapter name
            ChapterId m_id; //!< Chapter ID
        };

        /**
         * Node data (this can not be copied and is thus wrapped into a shared_ptr, see NodeData)
         */
        using NodeDataUnique = std::variant<InvisibleRootData, ProjectHeadData, NotebookHeadData, SceneData, ChapterData>;

        /**
         * Data of any node. Instances of this type are shared resources.
         */
        using NodeData = std::shared_ptr<NodeDataUnique>;

        /**
         * Default-constructs a model with empty title and author and using the language "en_EN".
         */
        ProjectModel() noexcept;

        /**
         * Construct a new project
         * @param properties Bundles properties such as name and author
         * @param parent Parent object, may be nullptr
         */
        explicit ProjectModel(ProjectProperties const& properties, QObject* parent = nullptr) noexcept;

        /**
         * @return Current properties
         */
        ProjectProperties const& properties() const;

        /**
         * Replace project properties
         * @param properties New properties
         */
        void setProperties(ProjectProperties const& properties);

        /**
         * @return Index of the project root node
         */
        QModelIndex projectRootIndex() const;

        /**
         * @return Index of the notebook node
         */
        QModelIndex notebookIndex() const;

        QModelIndex index(int row, int column, QModelIndex const& parent) const override;

        QModelIndex parent(QModelIndex const& child) const override;

        int rowCount(QModelIndex const& parent) const override;

        int columnCount(QModelIndex const& parent) const override;

        QVariant data(QModelIndex const& index, int role) const override;

        bool setData(QModelIndex const& index, QVariant const& value, int role) override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role) override;

        Qt::ItemFlags flags(QModelIndex const& index) const override;

        Qt::DropActions supportedDropActions() const override;

        Qt::DropActions supportedDragActions() const override;

        QStringList mimeTypes() const override;

        QMimeData* mimeData(const QModelIndexList& indexes) const override;

        bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                const QModelIndex& parent) override;

        /**
         * @param index Valid index corresponding to a node
         * @return The type of that node
         */
        NodeType nodeType(QModelIndex const& index) const;

        /**
         * @param index Valid index corresponding to a node
         * @return Data of the node as a variant
         */
        NodeData const& nodeData(QModelIndex const& index) const;

        /**
         * Loads a scene from hard disk or creates a new one, if there is no file on hard disk
         * @param index Index of the scene
         * @return Pointer to the loaded scene document
         */
        SceneDocument* loadScene(QModelIndex const& index);

        /**
         * Unloads a scene if it is currently loaded, discarding all unsaved modifications
         * @param index Index of the scene
         */
        void unloadScene(QModelIndex const& index);

        /**
         * @param index Model index
         * @return True in case the node has been modified since last save, otherwise false
         */
        bool isContentModified(QModelIndex const& index) const;

        /**
         * @return True in case the model structure has been modified since last save, otherwise false
         */
        bool isStructureModified() const;

        /**
         * @return True in case the model has been modified since last save, otherwise false
         */
        bool isModified() const;

        bool moveRows(QModelIndex const& sourceParent, int sourceRow, int count, QModelIndex const& destinationParent,
                int destinationChild) override;

        /**
         * Inserts a single row of a certain type
         * @details If you try to insert rows in an invalid fashion, e.g. a chapter as child of a scene, then this
         *          method will fail.
         * @param row Row to insert at
         * @param type Row type, e.g. "chapter"
         * @param name Initial name of the row
         * @param parent Parent node. Defaults to invisible root node if nullptr
         * @return true in case of success, otherwise false
         */
        bool
        insertRow(int row, InsertableNodeType type, QString const& name, QModelIndex const& parent = QModelIndex());

        /**
         * Inserts a number of rows of a certain type
         * @details If you try to insert rows in an invalid fashion, e.g. a chapter as child of a scene, then this
         *          method will fail.
         * @param row Row to insert at
         * @param count Amount of rows to insert
         * @param type Row type, e.g. "chapter"
         * @param name Initial name of all inserted rows
         * @param parent Parent node. Defaults to invisible root node if nullptr
         * @return true in case of success, otherwise false
         */
        bool
        insertRows(int row, int count, InsertableNodeType type, QString const& name,
                QModelIndex const& parent = QModelIndex());

        /**
         * Remove rows and notify the undo-redo-system.
         * @param row First row to remove
         * @param count Amount of rows to remove
         * @param parent Parent index
         * @return true in case of success, otherwise false
         */
        bool removeRows(int row, int count, QModelIndex const& parent) override;

        /**
         * Erases all content
         */
        void clear();

        /**
         * Checks, whether nodes can be inserted at a specified location
         * @param parent Index of the parent node to insert under
         * @return true in case it can be inserted, otherwise false
         */
        bool canInsert(QModelIndex const& parent = QModelIndex()) const;

        /**
         * Checks, whether children of a node can be removed
         * @param parent Parent node
         * @return true in case children can be removed, otherwise false
         */
        bool canRemove(QModelIndex const& parent = QModelIndex()) const;

        /**
         * Read project data from file
         * @param file Project file
         * @return true in case of success, otherwise false
         */
        bool read(QFile& file);

        /**
         * Read project data from XML string
         * @param xml XML description of the project
         * @return true in case of success, otherwise false
         */
        bool read(QString const& xml);

        /**
         * Write project data to file
         * @param file File to write to
         * @param contentDir Directory where the actual content is stored
         * @return true in case of success, otherwise false
         */
        bool write(QFile& file) const;

        /**
         * Write project data to XML string
         * @param xml String to write to
         * @param contentDir Directory where the actual content is stored
         * @return true in case of success, otherwise false
         */
        bool write(QString& xml) const;

        /**
         * Opens a project from hard disk
         * @param dir Directory containing the project
         * @return True in case of success, otherwise false
         */
        bool open(QDir const& dir);

        /**
         * Saves the project and all content to hard disk (where it was loaded from)
         * @return True in case of success, otherwise false
         */
        bool save();

        /**
         * Change the save directory of the project
         * @param dir New directory
         */
        void setSaveDir(QDir const& dir);

        /**
         * @return Current save directory
         */
        QDir const& saveDir() const;

        /**
         * @return Content directory
         */
        QDir contentDir() const;

        /**
         * @return true if the project was never saved to it's current saveDir, otherwise false
         */
        bool neverSaved() const;

        /**
         * Print project model to stream in a human-readable format
         * @param stream Stream to write to
         * @param model Model to write
         * @return The stream
         */
        friend std::ostream& operator<<(std::ostream& stream, ProjectModel const& model);

    signals:
        /**
         * Called whenever an item is about to be removed
         * @param idx Index of the node
         * @param type Node type
         */
        void beforeItemRemoved(QModelIndex const& idx, NodeType type);

    private:
        using Node = TreeNode<NodeData>;

        IdManager<Chapter_Tag> m_chapterIdMgr;
        IdManager<Scene_Tag> m_sceneIdMgr;
        Node m_root{std::make_shared<NodeDataUnique>(InvisibleRootData{})};
        QDir m_saveDir;
        bool m_neverSaved = true;
        QString const m_contentDirName = "content";
        bool m_modified = false;
        QUndoStack m_undoStack;

        void createRootNodes(ProjectProperties const& properties);

        bool readInternal(QXmlStreamReader& xml);

        bool readChapterOrScene(QXmlStreamReader& xml, QModelIndex parent);

        bool moveRowInternal(QModelIndex const& sourceParent, int sourceRow, QModelIndex const& destinationParent,
                int destinationRow);

        /**
         * Remove a row without notifying the undo-redo-system
         * @param row Row to remove
         * @param parent Valid parent index
         */
        void doRemoveRow(int row, QModelIndex const& parent);

        /**
         * Insert a row without notifying the undo-redo-system
         * @param n Node to insert
         * @param row Row to insert at
         * @param parent Valid parent index
         */
        void doInsertRow(Node n, int row, QModelIndex const& parent);

        void writeChapterOrScene(QXmlStreamWriter& xml, QModelIndex item) const;

        static NodeType nodeType(Node const& n);

        static NodeType nodeType(NodeData const& nodeData);

        NodeData makeNodeData(InsertableNodeType type, QString const& name);

        QModelIndexList childIndices(QModelIndex const& parent) const;

        QModelIndexList childIndices(Node const& n) const;

        friend std::ostream& operator<<(std::ostream& stream, NodeData const& nodeData);

        friend InsertRowCommand;

    private slots:

        void onDataChanged(QModelIndex const& topLeft, QModelIndex const& bottomRight, QVector<int> const& roles);

        void onLayoutChanged(QList<QPersistentModelIndex> const& parents, QAbstractItemModel::LayoutChangeHint hint);

        void onRowsInserted(QModelIndex const& parent, int first, int last);

        void onRowsRemoved(QModelIndex const& parent, int first, int last);
    };

    class InsertRowCommand : public QUndoCommand {
    private:
        using NodeData = ProjectModel::NodeData;
        using Node = TreeNode<NodeData>;

    public:
        InsertRowCommand(NodeData data, QModelIndex const& parentIdx, int row, ProjectModel* model, QUndoCommand *parent = nullptr);

        void undo() override;

        void redo() override;

    private:
        NodeData m_data;
        ModelPath m_path;
        int m_row;
        ProjectModel* m_model;
    };
}

#endif //NOVELIST_PROJECTMODEL_H
