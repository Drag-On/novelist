/**********************************************************
 * @file   ProjectModel.cpp
 * @author jan
 * @date   17.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QDebug>
#include <QFile>
#include <QUuid>
#include <QTextDocument>
#include <QStack>
#include <QTextDocumentWriter>
#include "model/ProjectModel.h"

namespace novelist {
    ProjectModel::ProjectModel() noexcept
            :ProjectModel(nullptr, "", "", Language("en_EN"))
    {
    }

    ProjectModel::ProjectModel(QObject* parent, QString const& name, QString author,
            Language lang) noexcept
            :QStandardItemModel(parent),
             m_author(std::move(author)),
             m_lang(std::move(lang))
    {
        // Load icons
        initIcons();

        // Add (visible) root
        QStandardItem* root = createProjectRoot();
        root->setText(name);
    }

    ProjectModel::ProjectModel(QObject* parent, QString const& filename)
            :QStandardItemModel(parent)
    {
        // Load icons
        initIcons();

        // Try to open project from file
        if (!read(filename))
            qWarning() << filename << "is not a valid project.";
    }

    bool ProjectModel::read(QString const& filename)
    {
        // Clear previous data
        clear();

        // Add root item
        createProjectRoot();

        // Read in project model
        QFile file(filename + "/project.xml");
        file.open(QIODevice::ReadOnly);

        QXmlStreamReader xml(&file);
        if (xml.readNextStartElement()) {
            if (xml.name() == "novel" && xml.attributes().value("version") == "1.0")
                if (readInternal(xml, filename))
                    return true;
        }
        if (xml.hasError())
            qWarning() << "Error while reading project file." << xml.errorString() << "At line" << xml.lineNumber()
                       << ", column" << xml.columnNumber() << ", character offset" << xml.characterOffset();

        return false;
    }

    bool ProjectModel::write(QString const& filename)
    {
        QFile file(filename + "/project.xml");
        if (!file.open(QIODevice::WriteOnly))
            return false;

        QStandardItem* root = invisibleRootItem()->child(0);

        // Write project xml file
        //
        QXmlStreamWriter xml(&file);
        xml.setAutoFormatting(true);

        xml.writeStartDocument();
        xml.writeDTD("<!DOCTYPE novel>");
        xml.writeStartElement("novel");
        xml.writeAttribute("version", "1.0");

        // Write project root information
        xml.writeStartElement("meta");
        xml.writeAttribute("name", root->text());
        xml.writeAttribute("author", m_author);
        xml.writeAttribute("lang", m_lang.shortname());
        xml.writeEndElement();

        // Write chapters and documents
        xml.writeStartElement("content");
        for (int r = 0; r < root->rowCount(); ++r)
            writeChapterDoc(xml, root->child(r));
        xml.writeEndElement();

        xml.writeEndDocument();

        // Write all documents to ./content/*
        //
        QStack<QStandardItem*> itemStack;
        for (int i = 0; i < root->rowCount(); ++i)
            itemStack.push_back(root->child(i));
        while (!itemStack.isEmpty()) {
            QStandardItem* item = itemStack.pop();
            NodeType type = item->data(NodeTypeRole).value<NodeType>();
            if (type == NodeType::Document) {
                QTextDocument* doc = reinterpret_cast<QTextDocument*>(item->data(DocumentRole).value<quintptr>());
                QUuid uid = item->data(UIDRole).value<QUuid>();
                QTextDocumentWriter writer(filename + "/content/" + uid.toString() + ".html", "HTML");
                writer.write(doc);
            }
            else if (type == NodeType::Chapter) {
                for (int i = 0; i < item->rowCount(); ++i)
                    itemStack.push_back(item->child(i));
            }
        }

        return true;
    }

    QString ProjectModel::name() const
    {
        Q_ASSERT(invisibleRootItem()->hasChildren());
        Q_ASSERT(invisibleRootItem()->rowCount() == 1);

        QStandardItem* root = invisibleRootItem()->child(0);

        return root->text();
    }

    void ProjectModel::setName(QString const& name)
    {
        Q_ASSERT(invisibleRootItem()->hasChildren());
        Q_ASSERT(invisibleRootItem()->rowCount() == 1);

        QStandardItem* root = invisibleRootItem()->child(0);
        root->setText(name);
    }

    QString ProjectModel::author() const
    {
        return m_author;
    }

    void ProjectModel::setAuthor(QString const& author)
    {
        m_author = author;
    }

    Language ProjectModel::lang() const
    {
        return m_lang;
    }

    void ProjectModel::setLang(Language const& lang)
    {
        m_lang = lang;
    }

    QStandardItem* ProjectModel::projectRoot()
    {
        Q_ASSERT(invisibleRootItem()->hasChildren());
        Q_ASSERT(invisibleRootItem()->rowCount() == 1);

        return invisibleRootItem()->child(0);
    }

    QModelIndex ProjectModel::createChapter(QModelIndex const& idx, QString const& name)
    {
        QStandardItem* item = itemFromIndex(idx);
        QStandardItem* parent = item->parent();

        QStandardItem* newChapter = createChapterItem(name);

        // If the selected node is the (visible) root node, append new chapter
        if (parent == invisibleRootItem() || parent == nullptr)
            invisibleRootItem()->child(0)->appendRow(newChapter);
        else
            parent->insertRow(item->row() + 1, newChapter);

        return indexFromItem(newChapter);
    }

    QModelIndex ProjectModel::createChapter(QString const& name)
    {
        Q_ASSERT(invisibleRootItem()->hasChildren());
        Q_ASSERT(invisibleRootItem()->rowCount() == 1);

        QStandardItem* root = invisibleRootItem()->child(0);

        // Add item to the end
        QStandardItem* newChapter = createChapterItem(name);
        root->appendRow(newChapter);

        return indexFromItem(newChapter);
    }

    QModelIndex ProjectModel::createDocument(QModelIndex const& idx, QString const& name)
    {
        QStandardItem* item = itemFromIndex(idx);
        QStandardItem* parent = item->parent();

        QStandardItem* newDoc = createDocumentItem(name, QUuid::createUuid());

        // If the selected node is the (visible) root node, append new chapter
        if (parent == invisibleRootItem() || parent == nullptr)
            invisibleRootItem()->child(0)->appendRow(newDoc);
        else
            parent->insertRow(item->row() + 1, newDoc);

        return indexFromItem(newDoc);
    }

    QModelIndex ProjectModel::createDocument(QString const& name)
    {
        Q_ASSERT(invisibleRootItem()->hasChildren());
        Q_ASSERT(invisibleRootItem()->rowCount() == 1);

        QStandardItem* root = invisibleRootItem()->child(0);

        // Add item to the end
        QStandardItem* newDoc = createDocumentItem(name, QUuid::createUuid());
        root->appendRow(newDoc);

        return indexFromItem(newDoc);
    }

    QStandardItem* ProjectModel::createProjectRoot()
    {
        auto* root = new QStandardItem();
        root->setIcon(m_iconRoot);
        root->setData(QVariant::fromValue(NodeType::ProjectRoot), NodeTypeRole);
        root->setDragEnabled(false);
        invisibleRootItem()->appendRow(root);
        invisibleRootItem()->setDropEnabled(false);

        return root;
    }

    QStandardItem* ProjectModel::createDocumentItem(QString const& name, QUuid const& uid)
    {
        auto* newDoc = new QStandardItem();
        newDoc->setText(name);
        newDoc->setIcon(m_iconDocument);
        newDoc->setData(QVariant::fromValue(NodeType::Document), NodeTypeRole);
        newDoc->setData(QVariant::fromValue(uid), UIDRole);
        newDoc->setData(QVariant::fromValue(reinterpret_cast<quintptr>(new QTextDocument(this))), DocumentRole);
        newDoc->setDropEnabled(false);
        return newDoc;
    }

    QStandardItem* ProjectModel::createChapterItem(QString const& name)
    {
        auto* newChapter = new QStandardItem();
        newChapter->setText(name);
        newChapter->setIcon(m_iconChapter);
        newChapter->setData(QVariant::fromValue(NodeType::Chapter), NodeTypeRole);

        return newChapter;
    }

    bool ProjectModel::readInternal(QXmlStreamReader& xml, QString const& filename)
    {
        Q_ASSERT(xml.isStartElement() && xml.name() == "novel");

        while (xml.readNextStartElement()) {
            if (xml.name() == "meta") {
                if (!readMeta(xml))
                    return false;
            }
            else if (xml.name() == "content") {
                if (!readContent(xml, filename))
                    return false;
            }
            else
                xml.skipCurrentElement();
        }
        return true;
    }

    bool ProjectModel::readMeta(QXmlStreamReader& xml)
    {
        Q_ASSERT(xml.isStartElement() && xml.name() == "meta");
        Q_ASSERT(invisibleRootItem()->hasChildren());
        Q_ASSERT(invisibleRootItem()->rowCount() == 1);

        QStandardItem* root = invisibleRootItem()->child(0);

        if (xml.attributes().hasAttribute("name"))
            root->setText(xml.attributes().value("name").toString());
        if (xml.attributes().hasAttribute("author"))
            m_author = xml.attributes().value("author").toString();
        if (xml.attributes().hasAttribute("lang"))
            m_lang = Language(xml.attributes().value("lang").toString());

        xml.skipCurrentElement();

        return true;
    }

    bool ProjectModel::readContent(QXmlStreamReader& xml, QString const& filename)
    {
        Q_ASSERT(xml.isStartElement() && xml.name() == "content");
        Q_ASSERT(invisibleRootItem()->hasChildren());
        Q_ASSERT(invisibleRootItem()->rowCount() == 1);

        QStandardItem* root = invisibleRootItem()->child(0);

        while (xml.readNextStartElement()) {
            if (!readChapterDoc(xml, indexFromItem(root), filename))
                return false;
        }

        return true;
    }

    bool ProjectModel::readChapterDoc(QXmlStreamReader& xml, QModelIndex parent, QString const& filename)
    {
        Q_ASSERT(xml.isStartElement() && (xml.name() == "chapter" || xml.name() == "document"));
        Q_ASSERT(parent.isValid());

        if (xml.name() == "chapter") {
            QString name;
            if (xml.attributes().hasAttribute("name"))
                name = xml.attributes().value("name").toString();
            QStandardItem* chapter = createChapterItem(name);
            itemFromIndex(parent)->appendRow(chapter);
            while (xml.readNextStartElement()) {
                if (!readChapterDoc(xml, indexFromItem(chapter), filename))
                    return false;
            }
        }
        else if (xml.name() == "document") {
            QString name;
            QUuid uid;
            if (xml.attributes().hasAttribute("name"))
                name = xml.attributes().value("name").toString();
            if (xml.attributes().hasAttribute("uid"))
                uid = QUuid(xml.attributes().value("uid").toString());
            QStandardItem* doc = createDocumentItem(name, uid);

            // Read in document content
            QFile file(filename + "/content/" + uid.toString() + ".html");
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString text = file.readAll();
                QTextDocument* docPtr = reinterpret_cast<QTextDocument*>(doc->data(DocumentRole).value<quintptr>());
                docPtr->setHtml(text);
            }

            itemFromIndex(parent)->appendRow(doc);
            xml.skipCurrentElement();
        }
        else {
            xml.skipCurrentElement();
        }

        return true;
    }

    void ProjectModel::writeChapterDoc(QXmlStreamWriter& xml, QStandardItem const* pItem)
    {
        auto type = pItem->data(NodeTypeRole).value<NodeType>();
        switch (type) {
        case NodeType::Chapter: {
            xml.writeStartElement("chapter");
            xml.writeAttribute("name", pItem->text());
            for (int r = 0; r < pItem->rowCount(); ++r)
                writeChapterDoc(xml, pItem->child(r));
            xml.writeEndElement();
            break;
        }
        case NodeType::Document: {
            xml.writeStartElement("document");
            xml.writeAttribute("name", pItem->text());
            xml.writeAttribute("uid", pItem->data(UIDRole).toString());
            xml.writeEndElement();
            break;
        }
        default:
            break;
        }
    }

    void ProjectModel::initIcons()
    {
        QString const rootIconTheme = QStringLiteral("go-home");
        if (QIcon::hasThemeIcon(rootIconTheme))
            m_iconRoot = QIcon::fromTheme(rootIconTheme);
        QString const chapterIconTheme = QStringLiteral("text-x-generic-template");
        if (QIcon::hasThemeIcon(chapterIconTheme))
            m_iconChapter = QIcon::fromTheme(chapterIconTheme);
        QString const documentIconTheme = QStringLiteral("text-x-generic");
        if (QIcon::hasThemeIcon(documentIconTheme))
            m_iconDocument = QIcon::fromTheme(documentIconTheme);
    }
}

QDataStream& operator<<(QDataStream& out, novelist::ProjectModel::NodeType const& nodeType)
{
    out << static_cast<int>(nodeType);
    return out;
}

QDataStream& operator>>(QDataStream& in, novelist::ProjectModel::NodeType& nodeType)
{
    int type;
    in >> type;
    nodeType = static_cast<novelist::ProjectModel::NodeType>(type);
    return in;
}
