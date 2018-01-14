/**********************************************************
 * @file   FilePicker.h
 * @author jan
 * @date   11/4/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_FILEPICKER_H
#define NOVELIST_FILEPICKER_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFileDialog>
#include <novelist_core_export.h>

namespace novelist {
    /**
     * A simple file picker widget, consisting of a line edit that shows the path and a button to browse for it
     */
    class NOVELIST_CORE_EXPORT FilePicker : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString selectedFile MEMBER m_selectedFile READ selectedFile WRITE setSelectedFile)
    Q_PROPERTY(QFileDialog::FileMode fileMode MEMBER m_fileMode READ fileMode WRITE setFileMode)
    Q_PROPERTY(QFileDialog::AcceptMode acceptMode MEMBER m_acceptMode READ acceptMode WRITE setAcceptMode)
    Q_PROPERTY(QFileDialog::ViewMode viewMode MEMBER m_viewMode READ viewMode WRITE setViewMode)

    public:
        /**
         * Construct widget
         * @param parent Parent widget
         * @param f Flags
         */
        explicit FilePicker(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) noexcept;

        /**
         * @return Currently selected path
         * @note This might be an invalid path, if the user types into the box manually
         */
        QString selectedFile() const noexcept;

        /**
         * @param filename New path to put into the line edit
         */
        void setSelectedFile(QString const& filename) noexcept;

        /**
         * @return Current file mode
         */
        QFileDialog::FileMode fileMode() const noexcept;

        /**
         * @param fileMode New file mode
         */
        void setFileMode(QFileDialog::FileMode fileMode) noexcept;

        /**
         * @return Current accept mode
         */
        QFileDialog::AcceptMode acceptMode() const noexcept;

        /**
         * @param acceptMode New accept mode
         */
        void setAcceptMode(QFileDialog::AcceptMode acceptMode) noexcept;

        /**
         * @return Current view mode
         */
        QFileDialog::ViewMode viewMode() const noexcept;

        /**
         * @param viewMode New view mode
         */
        void setViewMode(QFileDialog::ViewMode viewMode) noexcept;

    private:
        QLineEdit* m_lineEdit;
        QPushButton* m_button;
        QFileDialog::FileMode m_fileMode = QFileDialog::FileMode::AnyFile;
        QFileDialog::AcceptMode m_acceptMode = QFileDialog::AcceptMode::AcceptOpen;
        QFileDialog::ViewMode m_viewMode = QFileDialog::ViewMode::Detail;
        QFileDialog::Options m_options = 0;

        void onFilePick();
    };
}

#endif //NOVELIST_FILEPICKER_H
