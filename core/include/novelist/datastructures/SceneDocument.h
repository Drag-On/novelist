/**********************************************************
 * @file   SceneDocument.h
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SCENEDOCUMENT_H
#define NOVELIST_SCENEDOCUMENT_H

#include <QTextDocument>
#include <QTextFragment>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace novelist {
    /**
     * Formatted text for scenes
     */
    class SceneDocument : public QTextDocument {
    Q_OBJECT

    public:
        using QTextDocument::QTextDocument;

        /**
         * Read the scene from file
         * @param file File to read from
         * @return true in case of success, otherwise false
         */
        bool read(QFile& file);

        /**
         * Read the scene from xml definition
         * @param xml XML code
         * @return true in case of success, otherwise false
         */
        bool read(QString const& xml);

        /**
         * Write the scene to file
         * @param file File to write to
         * @return true in case of success, otherwise false
         */
        bool write(QFile& file) const;

        /**
         * Write the scene to a string
         * @param[out] xml String to write to
         * @return true in case of success, otherwise false
         */
        bool write(QString& xml) const;

    private:
        bool readInternal(QXmlStreamReader& xml);

        bool readBlock(QXmlStreamReader& xml, QTextCursor& cursor);

        bool readText(QXmlStreamReader& xml, QTextCursor& cursor);

        bool writeBlock(QXmlStreamWriter& xml, QTextBlock const& block) const;

        bool writeFragment(QXmlStreamWriter& xml, QTextFragment const& fragment) const;
    };
}

#endif //NOVELIST_SCENEDOCUMENT_H
