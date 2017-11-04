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

namespace novelist {
    /**
     * A simple file picker widget, consisting of a line edit that shows the path and a button to browse for it
     */
    class FilePicker : public QWidget {
    Q_OBJECT

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
