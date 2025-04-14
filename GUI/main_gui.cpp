#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QString>

extern "C" {
#include "../include/secure_aes_gcm.h"
}

class EncryptorWindow : public QWidget {
    Q_OBJECT
public:
    EncryptorWindow(QWidget *parent=nullptr) : QWidget(parent) {
        setWindowTitle("AES-256-GCM Encryptor");

        auto *layout = new QVBoxLayout(this);

        // Passphrase
        auto *passLayout = new QHBoxLayout;
        passLayout->addWidget(new QLabel("Passphrase:", this));
        m_passEdit = new QLineEdit(this);
        m_passEdit->setEchoMode(QLineEdit::Password);
        passLayout->addWidget(m_passEdit);
        layout->addLayout(passLayout);

        // Label fichier
        m_fileLabel = new QLabel("Aucun fichier sélectionné", this);
        layout->addWidget(m_fileLabel);

        // Boutons
        auto *btnLayout = new QHBoxLayout;
        auto *btnSelect = new QPushButton("Sélectionner Fichier", this);
        auto *btnEncrypt = new QPushButton("Chiffrer", this);
        auto *btnDecrypt = new QPushButton("Déchiffrer", this);
        btnLayout->addWidget(btnSelect);
        btnLayout->addWidget(btnEncrypt);
        btnLayout->addWidget(btnDecrypt);
        layout->addLayout(btnLayout);

        connect(btnSelect, &QPushButton::clicked, this, &EncryptorWindow::onSelectFile);
        connect(btnEncrypt, &QPushButton::clicked, this, &EncryptorWindow::onEncrypt);
        connect(btnDecrypt, &QPushButton::clicked, this, &EncryptorWindow::onDecrypt);
    }

private slots:
    void onSelectFile() {
        QString fileName = QFileDialog::getOpenFileName(this, "Choisir un fichier");
        if (!fileName.isEmpty()) {
            m_selectedFile = fileName;
            m_fileLabel->setText("Fichier: " + fileName);
        }
    }

    void onEncrypt() {
        if (m_selectedFile.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Aucun fichier sélectionné !");
            return;
        }
        if (m_passEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Passphrase vide !");
            return;
        }
        QString outFile = QFileDialog::getSaveFileName(this, "Fichier chiffré");
        if (outFile.isEmpty()) return;

        bool ok = secure_aes_gcm_encrypt_file(
            m_selectedFile.toStdString().c_str(),
            outFile.toStdString().c_str(),
            m_passEdit->text().toStdString().c_str());

        if (ok) QMessageBox::information(this, "OK", "Fichier chiffré avec succès !");
        else QMessageBox::critical(this, "Erreur", "Echec chiffrement (voir logs)");
    }

    void onDecrypt() {
        if (m_selectedFile.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Aucun fichier sélectionné !");
            return;
        }
        if (m_passEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Passphrase vide !");
            return;
        }
        QString outFile = QFileDialog::getSaveFileName(this, "Fichier déchiffré");
        if (outFile.isEmpty()) return;

        bool ok = secure_aes_gcm_decrypt_file(
            m_selectedFile.toStdString().c_str(),
            outFile.toStdString().c_str(),
            m_passEdit->text().toStdString().c_str());

        if (ok) QMessageBox::information(this, "OK", "Fichier déchiffré avec succès !");
        else QMessageBox::critical(this, "Erreur", "Echec déchiffrement (tag invalide ?)");
    }

private:
    QLineEdit *m_passEdit;
    QLabel *m_fileLabel;
    QString m_selectedFile;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    EncryptorWindow w;
    w.show();
    return app.exec();
}

#include "main_gui.moc"
