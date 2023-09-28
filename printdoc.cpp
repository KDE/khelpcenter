/*
  SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "printdoc.h"

#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QWebEngineView>

#include <KLocalizedString>

PrintDoc::PrintDoc(QObject *parent)
    : QObject{parent}
{
}

PrintDoc::~PrintDoc() = default;

void PrintDoc::setView(QWebEngineView *view)
{
    Q_ASSERT(!mView);
    mView = view;
    connect(mView, &QWebEngineView::printRequested, this, &PrintDoc::printPreview);
    connect(mView, &QWebEngineView::printFinished, this, &PrintDoc::printFinished);
}

void PrintDoc::setDocumentName(const QString &name)
{
    mDocumentName = name;
}

void PrintDoc::print()
{
    QPrintDialog dialog(&mPrinter, mView);
    if (dialog.exec() != QDialog::Accepted) {
        deleteLater();
        return;
    }
    printDocument(&mPrinter);
}

void PrintDoc::printDocument(QPrinter *printer)
{
    mView->print(printer);
    mWaitForResult.exec();
}

void PrintDoc::printFinished(bool success)
{
    if (!success) {
        QPainter painter;
        if (painter.begin(&mPrinter)) {
            QFont font = painter.font();
            font.setPixelSize(20);
            painter.setFont(font);
            painter.drawText(QPointF(10, 25), i18n("Could not generate print preview."));
            painter.end();
        }
    }
    mWaitForResult.quit();
    Q_EMIT printingFinished();
}

QWidget *PrintDoc::parentWidget() const
{
    return mParentWidget;
}

void PrintDoc::setParentWidget(QWidget *newParentWidget)
{
    mParentWidget = newParentWidget;
}

QString PrintDoc::documentName() const
{
    return mDocumentName;
}

void PrintDoc::printPreview()
{
    if (!mView) {
        deleteLater();
        return;
    }
    if (mInPrintPreview) {
        deleteLater();
        return;
    }
    mInPrintPreview = true;

    QPrintPreviewDialog preview(&mPrinter, mParentWidget);
    preview.setWindowTitle(i18nc("@title:window", "Print Document"));
    preview.resize(800, 750);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &PrintDoc::printDocument);
    preview.exec();
    mInPrintPreview = false;
    deleteLater();
}

#include "moc_printdoc.cpp"
