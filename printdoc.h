/*
  SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QEventLoop>
#include <QObject>
#include <QPrinter>

class QWebEngineView;
class PrintDoc : public QObject
{
    Q_OBJECT
public:
    explicit PrintDoc(QObject *parent = nullptr);
    ~PrintDoc() override;

    void setView(QWebEngineView *view);
    void setDocumentName(const QString &name);

    void print();
    void printPreview();

    Q_REQUIRED_RESULT QString documentName() const;

    Q_REQUIRED_RESULT QWidget *parentWidget() const;
    void setParentWidget(QWidget *newParentWidget);

Q_SIGNALS:
    void printingFinished();

private:
    void printDocument(QPrinter *printer);
    void printFinished(bool success);
    QString mDocumentName;
    QPrinter mPrinter;
    QEventLoop mWaitForResult;
    QWidget *mParentWidget = nullptr;
    QWebEngineView *mView = nullptr;
    bool mInPrintPreview = false;
};
