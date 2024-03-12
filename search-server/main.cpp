// Код поисковой системы из финального задания спринта №2
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <optional>
#include <stdexcept>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    
    Document() = default;
    
    Document(int id_, double relevance_, int rating_) : id(id_), relevance(relevance_), rating(rating_)
    {}
    
    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

template <typename StringContainer>
set<string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    set<string> non_empty_strings;
    for (const string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}

class SearchServer {
public:
    
    SearchServer() = default;
    
    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words) {
        if (all_of(stop_words.begin(), stop_words.end(), IsValidWord)) {
            stop_words_ = MakeUniqueNonEmptyStrings(stop_words);
        }
        else {
            throw invalid_argument("слово содержит специальный символ"s);
        }
    }

    explicit SearchServer(const string& stop_words_text)
        : SearchServer(SplitIntoWords(stop_words_text))
    {
    }

    void AddDocument(int document_id, const string& document, DocumentStatus status,
                     const vector<int>& ratings) {
        if(documents_.count(document_id)){
            throw invalid_argument("Документ с добавленым раннее id");
        } 
        if(document_id < 0){
            throw invalid_argument("Документ с отрицательным id");
        } 
        if(IsValidWord(document) == false){
            throw invalid_argument("наличие недопустимых симоволов");
        }
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
        document_ids.push_back(document_id);
    }
    
    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query,
                                        DocumentPredicate document_predicate) const {
            vector<Document> result;
            const Query query = ParseQuery(raw_query);
            auto matched_documents = FindAllDocuments(query, document_predicate);
            sort(matched_documents.begin(), matched_documents.end(),
                 [](const Document& lhs, const Document& rhs) {
                     if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
                         return lhs.rating > rhs.rating;
                     } else {
                         return lhs.relevance > rhs.relevance;
                     }
                 });
            if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
                matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
            }
            result = matched_documents;
            return result;
        }
 
    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const {
            return FindTopDocuments(
                raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
                    return document_status == status;
                });
        }
 
    vector<Document> FindTopDocuments(const string& raw_query) const {
            return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
        }

    int GetDocumentCount() const {
        return documents_.size();
    }

     tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query,
                                                        int document_id) const {
        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
           auto result = tuple{matched_words, documents_.at(document_id).status};
            return result;
    }

        int GetDocumentId(const int index) const {
            return document_ids.at(index);
        }
    
private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;
    vector<int> document_ids;
    
    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }
    
    static bool IsValidWord(const string& word) {
        // A valid word must not contain special characters
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
        });
    }

    static bool IsValidMinus(int document_id, const string& document){
        for(int i = 0; i < document.size();i++){
        if(document[i] == '-' || (document[i] == '-' && document[i+1] == '-') || document[document.size()-1]== '-'){
            return false;
        }
        }
         return true;
    }
    
    static bool IsValidQuery(const string& raw_query) {
        if(IsValidWord(raw_query)==false) {
            return false;
        }
        for (int i = 0; i < raw_query.size(); ++i) {
            if (raw_query[i] == '-' || raw_query[raw_query.size()-1]=='-') {
                if (raw_query[i + 1] == '-' || raw_query[i + 1] == ' ') {
                    return false;
                }
            }
        } 
        return true;
    }
    
    
    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    static int ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int rating_sum = 0;
        for (const int rating : ratings) {
            rating_sum += rating;
        }
        return rating_sum / static_cast<int>(ratings.size());
    }

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(string text) const {
        if(text.empty()){
            throw ("Пустой текст");
        }
        
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        if(text.empty() || text[0] == '-' || !IsValidWord(text)){
            throw invalid_argument("Слишком много минусов");
        }
        return {text, is_minus, IsStopWord(text)};
    }

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                } else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    // Existence required
    double ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

    template<typename predicate>
    vector<Document> FindAllDocuments(const Query& query, predicate doc_pr) const {
        map<int, double> document_to_relevance;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto &[document_id, term_freq] : word_to_document_freqs_.at(word)) {
                if (doc_pr(document_id,documents_.at(document_id).status,documents_.at(document_id).rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto &[document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (const auto &[document_id, relevance] : document_to_relevance) {
            matched_documents.push_back(
                {document_id, relevance, documents_.at(document_id).rating});
        }
        return matched_documents;
    }
};

// ==================== для примера =========================
