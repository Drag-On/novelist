/**********************************************************
 * @file   Language.h
 * @author jan
 * @date   1/25/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NEW_NOVELIST_LANGUAGE_H
#define NEW_NOVELIST_LANGUAGE_H

#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <novelist_core_export.h>
#include <ostream>
#include <sstream>
#include <memory>
#include <unordered_map>

namespace novelist::editor {

    /**
     * Enumeration of all supported novel languages
     */
    enum class NOVELIST_CORE_EXPORT Language {
        English = 0,
        German,
    };

    /**
     * Enumeration of all countries whose languages (or dialects) are supported
     */
    enum class NOVELIST_CORE_EXPORT Country {
        UnitedStates = 0,
        UnitedKingdom,
        Australia,
        Germany,
        Austria,
        Switzerland,
    };

    /**
     * Write the name of a language to a stream
     * @param os Output stream
     * @param language Language
     * @return The output stream
     */
    NOVELIST_CORE_EXPORT std::ostream& operator<<(std::ostream& os, Language language) noexcept;
    /**
     * Write the name of a country to a stream
     * @param os Output stream
     * @param country Country
     * @return The output stream
     */
    NOVELIST_CORE_EXPORT std::ostream& operator<<(std::ostream& os, Country country) noexcept;

    /**
     * Quotation marks characters used by a language
     */
    struct NOVELIST_CORE_EXPORT QuotationMarks {
        std::pair<char const* const, char const* const> const primary; //!< Primary (outer) quotes
        std::pair<char const* const, char const* const> const secondary; //!< Secondary (inner) quotes
    };

    /**
     * Provides features associated with a language, such as its language code
     */
    struct NOVELIST_CORE_EXPORT LanguageFeatures {
        char const* const code; //!< Language code, e.g. "en" for english
        std::initializer_list<QuotationMarks> const quotes; //!< Used primary and secondary quotation marks
    };
    /**
     * Provides features associated with a country, such as its country code
     */
    struct NOVELIST_CORE_EXPORT CountryFeatures {
        char const* const code; //!< Country code, e.g. "US" for united states
    };

    /**
     * Global instance of language features
     * @tparam lang Language to get features for
     */
    template<Language lang>
    constexpr LanguageFeatures const languageFeatures;
    template<>
    constexpr LanguageFeatures const languageFeatures<Language::English>{
            "en", // language code
            {{{"“", "”"}, {"‘", "’"}}, {{"‘", "’"}, {"“", "”"}}}, // quotes
    };
    template<>
    constexpr LanguageFeatures const languageFeatures<Language::German>{
            "de", // language code
            {{{"„", "“"}, {"‚", "‘"}}, {{"»", "«"}, {"›", "‹"}}}, // quotes
    };

    /**
     * Global instance of country features
     * @tparam country Country to get features for
     */
    template<Country country>
    constexpr CountryFeatures const countryFeatures;
    template<>
    constexpr CountryFeatures const countryFeatures<Country::UnitedStates>{"US"};
    template<>
    constexpr CountryFeatures const countryFeatures<Country::UnitedKingdom>{"UK"};
    template<>
    constexpr CountryFeatures const countryFeatures<Country::Australia>{"AU"};
    template<>
    constexpr CountryFeatures const countryFeatures<Country::Germany>{"DE"};
    template<>
    constexpr CountryFeatures const countryFeatures<Country::Austria>{"AT"};
    template<>
    constexpr CountryFeatures const countryFeatures<Country::Switzerland>{"CH"};

    /**
     * Interface class for project languages, which is a combination of a language and a country
     */
    class NOVELIST_CORE_EXPORT ProjectLanguage {
    public:
        virtual ~ProjectLanguage() = default;

        /**
         * @return Language
         */
        virtual Language language() const noexcept = 0;

        /**
         * @return Country
         */
        virtual Country country() const noexcept = 0;

        /**
         * @return Identifier string, e.g. "en-US" for american english
         */
        virtual QString identifier() const noexcept = 0;

        /**
         * @return Country code
         */
        virtual QString countryCode() const noexcept = 0;

        /**
         * @return Language code
         */
        virtual QString languageCode() const noexcept = 0;

        /**
         * @return Natural language description, e.g. "English (United States)"
         */
        virtual QString description() const noexcept = 0;

        /**
         * @return Icon associated with this language, if any
         */
        virtual QIcon icon() const noexcept = 0;

        /**
         * @return Full country features
         */
        virtual CountryFeatures const& countryFeatures() const noexcept = 0;

        /**
         * @return Full language features
         */
        virtual LanguageFeatures const& languageFeatures() const noexcept = 0;
    };

    namespace internal {
        template<Language t_lang, Country t_country>
        class NOVELIST_CORE_EXPORT ProjectLanguageImpl : public ProjectLanguage {
        public:
            Language language() const noexcept override
            {
                return t_lang;
            }

            Country country() const noexcept override
            {
                return t_country;
            }

            QString identifier() const noexcept override
            {
                return languageCode() + "-" + countryCode();
            }

            QString countryCode() const noexcept override
            {
                return novelist::editor::countryFeatures<t_country>.code;
            }

            QString languageCode() const noexcept override
            {
                return novelist::editor::languageFeatures<t_lang>.code;
            }

            QString description() const noexcept override
            {
                std::ostringstream ss;
                ss << t_lang << "(" << t_country << ")";
                return QString::fromStdString(ss.str());
            }

            QIcon icon() const noexcept override
            {
                return QIcon::fromTheme("flag-" + countryCode().toLower());
            }

            CountryFeatures const& countryFeatures() const noexcept override
            {
                return novelist::editor::countryFeatures<t_country>;
            }

            LanguageFeatures const& languageFeatures() const noexcept override
            {
                return novelist::editor::languageFeatures<t_lang>;
            }
        };

        std::unordered_map<Language, std::vector<std::unique_ptr<ProjectLanguage>>>& getLanguageMap() noexcept;
    }

    /**
     * Checks whether a string syntactically looks like a complete sentence in a given language
     * @param str String to check
     * @param lang Language to consider
     * @return true if looks like a complete sentence, otherwise false
     */
    bool isCompleteSentence(QString const& str, ProjectLanguage const* lang) noexcept;

    /**
     * Get a project language based on the enumerators for language and country
     * @param lang Language enumerator
     * @param country Country enumerator
     * @return A pointer to the project language object or nullptr if the language-country combination is invalid.
     */
    ProjectLanguage const* getProjectLanguage(Language lang, Country country) noexcept;

    /**
     * Get the list of all valid countries for a language, where the first element is the default country.
     * @param lang Language enumerator
     * @return List of countries available for the passed language
     */
    std::vector<Country> getCountriesForLanguage(Language lang) noexcept;

#define MAKE_LANG_COUNTRY(lang, country) \
    using Language##lang##country = internal::ProjectLanguageImpl<Language::lang, Country::country>;\
    namespace internal { \
        struct { \
        int i{ [] { \
                internal::getLanguageMap()[Language::lang].emplace_back(std::make_unique<Language##lang##country>()); \
                return 0; \
             }() }; \
        }lang##_##country##_init; \
    }

    /*
     * Define valid language-country combinations. First mentioned country for every language is used as default.
     */
    MAKE_LANG_COUNTRY(English, UnitedStates)
    MAKE_LANG_COUNTRY(English, UnitedKingdom)
    MAKE_LANG_COUNTRY(English, Australia)
    MAKE_LANG_COUNTRY(German, Germany)
    MAKE_LANG_COUNTRY(German, Austria)
    MAKE_LANG_COUNTRY(German, Switzerland)
}

#endif //NEW_NOVELIST_LANGUAGE_H
