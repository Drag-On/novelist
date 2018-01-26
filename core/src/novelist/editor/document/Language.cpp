/**********************************************************
 * @file   Language.cpp
 * @author jan
 * @date   1/25/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

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
}