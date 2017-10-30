/**********************************************************
 * @file   LanguageToolInspector.cpp
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtNetwork>
#include "LanguageToolInspector.h"

namespace novelist {

    InspectionBlockResult LanguageToolInspector::inspect(QString const& text, Language lang) const noexcept
    {
        QNetworkAccessManager network;
        QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

        QNetworkRequest dest(QUrl("http://localhost:8081/v2/check"));
        dest.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-www-form-urlencoded");

        QUrlQuery request;
        request.addQueryItem("text", text);
        request.addQueryItem("language", lang::identifier(lang));
        QString requestStr = request.toString(QUrl::FullyEncoded);

        QByteArray response;
        QNetworkReply* reply = network.post(dest, requestStr.toUtf8());

        // Wait for response
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        response += reply->readAll();
        delete reply;

        // TODO: Parse response

        return InspectionBlockResult();
    }
}