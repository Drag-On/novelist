/**********************************************************
 * @file   ProjectModel.h
 * @author jan
 * @date   17.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_PROJECTMODEL_H
#define NOVELIST_PROJECTMODEL_H

#include <QString>
#include <QXmlStreamReader>
#include <QStandardItemModel>
#include "lang/Language.h"

namespace novelist {
    /**
     * Models the project hierarchy
     */
    class ProjectModel : public QStandardItemModel {
    Q_OBJECT

    public:
        /**
         * Constructs an empty project with empty name and author and "en_EN" as language
         */
        ProjectModel() noexcept;

        /**
         * New project model
         * @param parent Parent object
         * @param name Project name
         * @param author Project author
         * @param lang Project language
         */
        ProjectModel(QObject* parent, QString const& name, QString author, Language lang) noexcept;

        /**
         * Constructs a project from file
         * @param parent Parent object
         * @param filename Project folder
         */
        ProjectModel(QObject* parent, QString const& filename);

        /**
         * Reads a project from file
         * @param filename Project folder
         * @return True if read successfully, otherwise false
         */
        bool read(QString const& filename);

        /**
         * Writes the project to a folder
         * @param filename Folder to write to
         * @return True if written successfully, otherwise false
         */
        bool write(QString const& filename);

        /**
         * @return Project name
         */
        QString name() const;

        /**
         * @param name New project name
         */
        void setName(QString const& name);

        /**
         * @return Project author
         */
        QString author() const;

        /**
         * @param author New author
         */
        void setAuthor(QString const& author);

        /**
         * @return Project language
         */
        Language lang() const;

        /**
         * @param lang New language
         */
        void setLang(Language const& lang);

        /**
         * @return Project root item
         */
        QStandardItem* projectRoot();

        /**
         * Creates a new chapter after the specified item
         * @param idx Index of the item to insert after
         * @param name Name of the new chapter
         * @returns Index of the inserted chapter
         */
        QModelIndex createChapter(QModelIndex const& idx, QString const& name = tr("New Chapter"));

        /**
         * Creates a new chapter at the end
         * @param name Name of the new chapter
         * @returns Index of the inserted chapter
         */
        QModelIndex createChapter(QString const& name = tr("New Chapter"));

        /**
         * Creates a new document after the specified item
         * @param idx Index of the item to insert after
         * @param name Name of the new document
         * @returns Index of the inserted document
         */
        QModelIndex createDocument(QModelIndex const& idx, QString const& name = tr("New Document"));

        /**
         * Creates a new document at the end
         * @param name Name of the new document
         * @returns Index of the inserted document
         */
        QModelIndex createDocument(QString const& name = tr("New Document"));

        /**
         * Project node type
         */
        enum class NodeType {
            ProjectRoot,    //!< The root node
            Chapter,        //!< A chapter
            Document,       //!< A document
        };

        Q_ENUM(NodeType)

        /**
         * Custom roles
         */
        enum CustomRole {
            NodeTypeRole = Qt::UserRole + 1,    //!< Stores type as NodeType
            UIDRole = Qt::UserRole + 2,         //!< UID as QUuid
            DocumentRole = Qt::UserRole + 3,    //!< Document content as QTextDocument
        };

        Q_ENUM(CustomRole)

    private:
        QStandardItem* createProjectRoot();

        QStandardItem* createDocumentItem(QString const& name, QUuid const& uid);

        QStandardItem* createChapterItem(QString const& name);

        bool readInternal(QXmlStreamReader& xml, QString const& filename);

        bool readMeta(QXmlStreamReader& xml);

        bool readContent(QXmlStreamReader& xml, QString const& filename);

        bool readChapterDoc(QXmlStreamReader& xml, QModelIndex parent, QString const& filename);

        void writeChapterDoc(QXmlStreamWriter& xml, QStandardItem const* pItem);

        void initIcons();

        QIcon m_iconRoot;
        QIcon m_iconChapter;
        QIcon m_iconDocument;

        QString m_author = "";
        Language m_lang{"invalid"};
    };
}

QDataStream& operator<<(QDataStream& out, novelist::ProjectModel::NodeType const& nodeType);

QDataStream& operator>>(QDataStream& in, novelist::ProjectModel::NodeType& nodeType);

#endif //NOVELIST_PROJECTMODEL_H
