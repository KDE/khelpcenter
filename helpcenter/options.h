//-----------------------------------------------------------------------------
//
// KDE Help Options
//
// (c) Martin R. Jones 1996
//

#ifndef __HELP_OPTIONS_H__
#define __HELP_OPTIONS_H__

#include <qtabdialog.h>
#include <qcombobox.h>
#include <qstrlist.h>
#include <kconfig.h>


//-----------------------------------------------------------------------------

class KFontOptions : public QWidget
{
	Q_OBJECT

public:
	KFontOptions( QWidget *parent = NULL, const char *name = NULL );

public slots:
	void	slotApplyPressed();
	void	slotFontSize( int );
	void	slotStandardFont( const QString& n );
	void	slotFixedFont( const QString& n );

signals:
	void	fontSize( int );
	void	standardFont( const QString& );
	void	fixedFont( const QString& );

private:
	void	readOptions();
	void	getFontList( QStrList &list, const char *pattern );
	void	addFont( QStrList &list, const char* xfont );

private:
	int		fSize;
	QString	stdName;
	QString	fixedName;
	QStrList standardFonts;
	QStrList fixedFonts;
};

//-----------------------------------------------------------------------------

class KColorOptions : public QWidget
{
	Q_OBJECT
public:
	KColorOptions( QWidget *parent = NULL, const char *name = NULL );

signals:
	void	colorsChanged( const QColor &bg, const QColor &text,
			    const QColor &link, const QColor &vlink, const bool underline,
                const bool forceDefaults );

protected slots:
	void	slotApplyPressed();
	void	slotBgColorChanged( const QColor &col );
	void	slotTextColorChanged( const QColor &col );
	void	slotLinkColorChanged( const QColor &col );
	void	slotVLinkColorChanged( const QColor &col );
	void    slotUnderlineLinksChanged( bool uline );
	void    slotForceDefaultChanged( bool force );

private:
	void	readOptions();

private:
	QColor bgColor;
	QColor textColor;
	QColor linkColor;
	QColor vLinkColor;
	bool   underlineLinks;
    bool   forceDefault;
	bool   changed;
};

//-----------------------------------------------------------------------------

class KHelpOptionsDialog : public QTabDialog
{
	Q_OBJECT
public:
	KHelpOptionsDialog( QWidget *parent = NULL, const char *name = NULL );

public:
	KFontOptions *fontOptions;
	KColorOptions *colorOptions;
};


#endif		// __HELP_OPTIONS_H__

