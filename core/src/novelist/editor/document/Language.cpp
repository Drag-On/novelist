/**********************************************************
 * @file   Language.cpp
 * @author jan
 * @date   1/25/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QtCore/QRegularExpression>
#include "editor/document/Language.h"

namespace novelist::editor {
    std::ostream& operator<<(std::ostream& os, Language language) noexcept
    {
        switch (language) {
            case Language::English:
                os << QCoreApplication::translate("Language", "English").toStdString();
                break;
            case Language::German:
                os << QCoreApplication::translate("Language", "German").toStdString();
                break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, Country country) noexcept
    {
        switch (country) {
            case Country::UnitedStates:
                os << QCoreApplication::translate("Language", "United States").toStdString();
                break;
            case Country::UnitedKingdom:
                os << QCoreApplication::translate("Language", "United Kingdom").toStdString();
                break;
            case Country::Australia:
                os << QCoreApplication::translate("Language", "Australia").toStdString();
                break;
            case Country::Germany:
                os << QCoreApplication::translate("Language", "Germany").toStdString();
                break;
            case Country::Austria:
                os << QCoreApplication::translate("Language", "Austria").toStdString();
                break;
            case Country::Switzerland:
                os << QCoreApplication::translate("Language", "Switzerland").toStdString();
                break;
        }
        return os;
    }

    namespace internal {
        std::unordered_map<Language, std::vector<std::unique_ptr<ProjectLanguage>>>& getLanguageMap() noexcept
        {
            static std::unordered_map<Language, std::vector<std::unique_ptr<ProjectLanguage>>> map;
            return map;
        }
    }

    bool isCompleteSentence(QString const& str, ProjectLanguage const* lang) noexcept
    {
        QString regex(".*[.?!…");
        for (auto quotes : lang->languageFeatures().quotes)
            regex += quotes.primary.second;
        regex += "]$";
        QRegularExpression re(regex);
        return re.match(str).hasMatch();
    }

    ProjectLanguage const* getProjectLanguage(Language lang, Country country) noexcept
    {
        if (internal::getLanguageMap().count(lang) == 0)
            return nullptr;

        auto const& vec = internal::getLanguageMap().at(lang);
        auto iter = std::find_if(vec.begin(), vec.end(), [country](auto const& e) { return e->country() == country; });
        if (iter == vec.end())
            return nullptr;

        return iter->get();
    }

    std::vector<Country> getCountriesForLanguage(Language lang) noexcept
    {
        std::vector<Country> countries;
        if (internal::getLanguageMap().count(lang) > 0) {
            auto const& vec = internal::getLanguageMap().at(lang);
            for (auto const& e : vec)
                countries.push_back(e->country());
        }

        return countries;
    }

    size_t countSyllables(QString const& string, ProjectLanguage const& lang) noexcept
    {
        QRegExp regex{"[\\s]"};
        QStringList words = string.split(regex);
        size_t syllableCount = 0;
        QStringList vowels;
        switch (lang.language()) {
            case Language::English:
            {
                vowels = QStringList{"a", "e", "i", "o", "u"};
                break;
            }
            case Language::German:
            {
                vowels = QStringList{"a", "e", "i", "o", "u", "ä", "ö", "ü"};
                break;
            }
        }
        // This is a pretty bad approximation.
        // TODO: Improve, look at https://stackoverflow.com/questions/9096228/counting-syllables-in-a-word
        for (auto& w : words)
        {
            w = w.toLower();
            bool wasVowel = false;
            for (auto c : w)
            {
                if (vowels.contains(c))
                {
                    if (!wasVowel)
                    {
                        ++syllableCount;
                        wasVowel = true;
                    }
                }
                else
                    wasVowel = false;
            }
        }
        return syllableCount;
    }

    size_t countSentences(QString const& string, ProjectLanguage const& /*lang*/) noexcept
    {
        // TODO: This is a very rough approximation
        QRegExp regex{"[\\.:;\\!\\?]"};
        QStringList sentences = string.split(regex);
        return sentences.size();
    }

    size_t countWords(QString const& string, ProjectLanguage const& /*lang*/) noexcept
    {
        // TODO: This is a very rough approximation
        QRegExp regex{"[\\s]"};
        QStringList words = string.split(regex);
        return words.size();
    }
}