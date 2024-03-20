#pragma once

#include <deque>

#include "search_server.h"
#include "document.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

       template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
        AddRequest(result.size());
        return result;
        // напишите реализацию
    }
    
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status) {
        auto result = search_server_.FindTopDocuments(raw_query, status);
        AddRequest(result.size());
        return result;
        // напишите реализацию
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query) {
        auto result = search_server_.FindTopDocuments(raw_query);
        AddRequest(result.size());
        return result;
        // напишите реализацию
    }
    
    int GetNoResultRequests() const;

private:
    struct QueryResult {
        int req;
        uint64_t time;
        // определите, что должно быть в структуре
    };

    std::deque<QueryResult> requests_;
    const SearchServer& search_server_;
    int count_req_ = 0;
    uint64_t current_time_ = 0;
    const static int min_in_day_ = 1440;
    void AddRequest(int num_request);
};
