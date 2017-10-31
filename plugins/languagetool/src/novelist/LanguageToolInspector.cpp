/**********************************************************
 * @file   LanguageToolInspector.cpp
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtNetwork>
#include <document/SpellingInsight.h>
#include <document/GrammarInsight.h>
#include <document/TypographyInsight.h>
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
        auto releaseReply = gsl::finally([reply] { delete reply; });

        // Wait for response
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        response += reply->readAll();

        if (reply->error() != QNetworkReply::NetworkError::NoError) {
            qWarning() << "Communication with local LanguageTool server failed.";
            return InspectionBlockResult();
        }

        // Parse response
        QJsonDocument json = QJsonDocument::fromJson(response);
        InspectionBlockResult result = parseJsonResponse(json);

        return result;
    }

    InspectionBlockResult LanguageToolInspector::parseJsonResponse(QJsonDocument const& json) const noexcept
    {
        InspectionBlockResult result;

        if (json.isObject()) {
            auto root = json.object();
            auto matches = root.value("matches");
            auto matchesArr = matches.toArray();
            for (auto val : matchesArr) {
                auto obj = val.toObject();
                QString msg = obj.value("message").toString();
                QString shortMsg = obj.value("shortMessage").toString("");
                int pos = obj.value("offset").toInt();
                int length = obj.value("length").toInt();
                QJsonArray replacements = obj.value("replacements").toArray();
                QStringList replacementStrings;
                for (auto o : replacements)
                    replacementStrings.push_back(o.toObject().value("value").toString());
                QJsonObject context = obj.value("context").toObject();
                QString contextStr = context.value("text").toString();
                QJsonObject rule = obj.value("rule").toObject();

                InspectionInsight insight;
                QString aggregateMsg;
                if (!shortMsg.isEmpty())
                    aggregateMsg += shortMsg + "\n\n";
                aggregateMsg += msg + "\n";
                if (replacementStrings.isEmpty())
                    aggregateMsg += QObject::tr("No suggestions available.\n", "LanguageToolInspector");
                else {
                    aggregateMsg += QObject::tr("Suggestions: ", "LanguageToolInspector");
                    aggregateMsg += replacementStrings.first();
                    for (auto iter = replacementStrings.begin() + 1; iter != replacementStrings.end(); ++iter) {
                        aggregateMsg += ", " + *iter;
                    }
                    aggregateMsg += "\n";
                }
                aggregateMsg += QObject::tr("In this context: ", "LanguageToolInspector");
                aggregateMsg += contextStr;
                insight.m_factory = makeFactory(aggregateMsg, replacementStrings, rule);
                insight.m_left = pos;
                insight.m_right = pos + length;

                result.push_back(std::move(insight));
            }
        }
        return result;
    }

    std::unique_ptr<InsightFactory>
    LanguageToolInspector::makeFactory(QString const& msg, QStringList suggestions, QJsonObject const& rule) const noexcept
    {
        QString issueType = rule.value("issueType").toString("");
        QString catId = rule.value("category").toObject().value("id").toString("");

        if (issueType.contains(QRegExp("grammar|grammatic", Qt::CaseSensitivity::CaseInsensitive)))
            return std::make_unique<AutoInsightFactory<GrammarInsight>>(msg, suggestions);
        else if (issueType.contains(QRegExp("typographical|typography", Qt::CaseSensitivity::CaseInsensitive)))
            return std::make_unique<AutoInsightFactory<TypographyInsight>>(msg, suggestions);
        else
            return std::make_unique<AutoInsightFactory<SpellingInsight>>(msg, suggestions);
    }
}