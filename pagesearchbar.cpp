/*
    SPDX-FileCopyrightText: 2017, 2024 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "pagesearchbar.h"

#include "ui_pagesearchbar.h"

#include "view.h"

namespace KHC {

PageSearchBar::PageSearchBar(View* view, QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::PageSearchBar)
    , m_view(view)
{
    m_ui->setupUi(this);

    connect(m_ui->hideButton, &QToolButton::clicked,
            this, &PageSearchBar::hide);
    connect(m_ui->searchTextEdit, &QLineEdit::textEdited,
            this, &PageSearchBar::searchIncrementally);
    connect(m_ui->matchCaseCheckButton, &QAbstractButton::toggled,
            this, &PageSearchBar::searchIncrementally);
    connect(m_ui->searchTextEdit, &QLineEdit::returnPressed,
            this, &PageSearchBar::searchNext);
    connect(m_ui->nextButton, &QToolButton::clicked,
            this, &PageSearchBar::searchNext);
    connect(m_ui->previousButton, &QToolButton::clicked,
            this, &PageSearchBar::searchPrevious);
    // TODO: disable next/previous buttons if no (more) search hits, color coding in text field
}

PageSearchBar::~PageSearchBar() = default;

void PageSearchBar::searchNext()
{
    const QString text = m_ui->searchTextEdit->text();
    if (text.isEmpty()) {
        startSearch();
        return;
    }

    QWebEnginePage::FindFlags findFlags = {};
    if (m_ui->matchCaseCheckButton->isChecked()) {
        findFlags |= QWebEnginePage::FindCaseSensitively;
    }

    m_view->findText(text, findFlags);
}

void PageSearchBar::searchPrevious()
{
    const QString text = m_ui->searchTextEdit->text();
    if (text.isEmpty()) {
        startSearch();
        return;
    }

    QWebEnginePage::FindFlags findFlags = {QWebEnginePage::FindBackward};
    if (m_ui->matchCaseCheckButton->isChecked()) {
        findFlags |= QWebEnginePage::FindCaseSensitively;
    }

    m_view->findText(text, findFlags);
}

void PageSearchBar::startSearch()
{
    // if there is seleted text, assume the user wants to search that selected text
    // otherwise reuse any old term
    const QString selectedText = m_view->selectedText();
    if (!selectedText.isEmpty()) {
        m_ui->searchTextEdit->setText(selectedText);
    }

    show();
    m_ui->searchTextEdit->selectAll();
    m_ui->searchTextEdit->setFocus();
}

void PageSearchBar::searchIncrementally()
{
    QWebEnginePage::FindFlags findFlags = {};

    if (m_ui->matchCaseCheckButton->isChecked()) {
        findFlags |= QWebEnginePage::FindCaseSensitively;
    }

    // calling with changed text with added or removed chars at end will result in current
    // selection kept, if also matching new text
    // behaviour on changed case sensitivity though is advancing to next match even if current
    // would be still matching. as there is no control about currently shown match, nothing
    // we can do about it. thankfully case sensitivity does not happen too often, so should
    // not be too grave UX
    // at least with webengine 5.9.1 there is a bug when switching from no-casesensitivy to
    // casesensitivity, that global matches are not updated and the ones with non-matching casing
    // still active. no workaround so far.
    m_view->findText(m_ui->searchTextEdit->text(), findFlags);
}

void PageSearchBar::hideEvent(QHideEvent* event)
{
    // finish search
    // passing emptry string to reset search, as told in API docs
    m_view->findText(QString());

    QWidget::hideEvent(event);
}

}

#include "moc_pagesearchbar.cpp"
