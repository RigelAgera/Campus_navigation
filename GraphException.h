// GraphException.h - Custom exception hierarchy for graph operations
#ifndef GRAPHEXCEPTION_H
#define GRAPHEXCEPTION_H

#include <stdexcept>
#include <string>

// ==================== Base Exception ====================
class GraphException : public std::runtime_error {
public:
    explicit GraphException(const std::string& msg)
        : std::runtime_error(msg) {}
};

// ==================== Place Exceptions ====================
class PlaceNotFoundException : public GraphException {
public:
    explicit PlaceNotFoundException(const std::string& place_id)
        : GraphException("Place not found: " + place_id),
          place_id_(place_id) {}

    const std::string& getPlaceId() const { return place_id_; }

private:
    std::string place_id_;
};

class DuplicatePlaceException : public GraphException {
public:
    explicit DuplicatePlaceException(const std::string& place_id)
        : GraphException("Duplicate place: " + place_id),
          place_id_(place_id) {}

    const std::string& getPlaceId() const { return place_id_; }

private:
    std::string place_id_;
};

// ==================== Road Exceptions ====================
class RoadNotFoundException : public GraphException {
public:
    RoadNotFoundException(const std::string& from_id, const std::string& to_id)
        : GraphException("Road not found: " + from_id + " -> " + to_id),
          from_id_(from_id), to_id_(to_id) {}

    const std::string& getFromId() const { return from_id_; }
    const std::string& getToId() const { return to_id_; }

private:
    std::string from_id_;
    std::string to_id_;
};

class DuplicateRoadException : public GraphException {
public:
    DuplicateRoadException(const std::string& from_id, const std::string& to_id)
        : GraphException("Duplicate road: " + from_id + " -> " + to_id),
          from_id_(from_id), to_id_(to_id) {}

    const std::string& getFromId() const { return from_id_; }
    const std::string& getToId() const { return to_id_; }

private:
    std::string from_id_;
    std::string to_id_;
};

// ==================== Algorithm Exceptions ====================
class PathNotFoundException : public GraphException {
public:
    PathNotFoundException(const std::string& start_id, const std::string& end_id)
        : GraphException("No path found: " + start_id + " -> " + end_id),
          start_id_(start_id), end_id_(end_id) {}

    const std::string& getStartId() const { return start_id_; }
    const std::string& getEndId() const { return end_id_; }

private:
    std::string start_id_;
    std::string end_id_;
};

class InvalidTimeException : public GraphException {
public:
    explicit InvalidTimeException(const std::string& time_str)
        : GraphException("Invalid time format: " + time_str),
          time_str_(time_str) {}

    const std::string& getTimeStr() const { return time_str_; }

private:
    std::string time_str_;
};

// ==================== Validation Exceptions ====================
class InvalidFieldException : public GraphException {
public:
    explicit InvalidFieldException(const std::string& field_name)
        : GraphException("Invalid field: " + field_name),
          field_name_(field_name) {}

    const std::string& getFieldName() const { return field_name_; }

private:
    std::string field_name_;
};

class InvalidRoadStatusException : public GraphException {
public:
    explicit InvalidRoadStatusException(const std::string& status)
        : GraphException("Invalid road status: " + status),
          status_(status) {}

    const std::string& getStatus() const { return status_; }

private:
    std::string status_;
};

#endif // GRAPHEXCEPTION_H
