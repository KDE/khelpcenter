#include <klocale.h>

#include "searchformatter.h"

SearchFormatter::SearchFormatter()
{
}

SearchFormatter:: ~SearchFormatter()
{
}

QString SearchFormatter::header()
{
  return "<html><head><title>" + i18n("Search Results") +
         "</title></head><body>";
}

QString SearchFormatter::footer()
{
  return "</body></html>";
}

QString SearchFormatter::separator()
{
//  return "<table width=100%><tr><td bgcolor=\"#7B8962\">&nbsp;"
//         "</td></tr></table>";
  return "<hr>";
}

QString SearchFormatter::docTitle( const QString &title )
{
  return "<h2><font color=\"red\">" + title + "</font></h2>";
}

QString SearchFormatter::processResult( const QString &data )
{
  QString result;

  enum { Header, BodyTag, Body, Footer };

  int state = Header;

  for( uint i = 0; i < data.length(); ++i ) {
    QChar c = data[i];
    switch ( state ) {
      case Header:
        if ( c == '<' && data.mid( i, 5 ).lower() == "<body" ) {
          state = BodyTag;
          i += 4;
        }
        break;
      case BodyTag:
        if ( c == '>' ) state = Body;
        break;
      case Body:
        if ( c == '<' && data.mid( i, 7 ).lower() == "</body>" ) {
          state = Footer;
        } else {
          result.append( c );
        }
        break;
      case Footer:
        break;
      default:
        result.append( c );
        break;
    }
  }

//  kdDebug() << "Result:" << endl << result << endl;

  return result;
}
