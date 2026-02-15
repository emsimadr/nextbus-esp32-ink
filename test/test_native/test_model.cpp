#include <unity.h>
#include "model.h"

// ============================================================
// Test fixtures (embedded as string literals)
// Derived from consumer-kit/examples/ wrapped in /v1/board format
// ============================================================

static const char* BOARD_OK_REALTIME = R"({
  "as_of": "2026-02-13T12:35:00-05:00",
  "items": [
    {
      "key": "route_1_inbound",
      "label": "Route 1 - Harvard Sq",
      "route_id": "1",
      "stop_id": "place-harsq",
      "direction_id": 0,
      "walk_minutes": 4,
      "status": "ok",
      "arrival": {
        "time": "2026-02-13T12:41:00-05:00",
        "minutes": 6,
        "leave_in_minutes": 2,
        "source": "realtime",
        "trip_id": "trip-001"
      },
      "alternatives": [
        {
          "time": "2026-02-13T12:53:00-05:00",
          "minutes": 18,
          "leave_in_minutes": 14,
          "source": "realtime",
          "trip_id": "trip-002"
        },
        {
          "time": "2026-02-13T13:05:00-05:00",
          "minutes": 30,
          "leave_in_minutes": 26,
          "source": "realtime",
          "trip_id": "trip-003"
        }
      ]
    }
  ]
})";

static const char* BOARD_OK_SCHEDULE = R"({
  "as_of": "2026-02-13T13:03:00-05:00",
  "items": [
    {
      "key": "route_73_outbound",
      "label": "Route 73 - Harvard via Mount Auburn St",
      "route_id": "73",
      "stop_id": "place-harsq",
      "direction_id": 1,
      "walk_minutes": 5,
      "status": "ok",
      "arrival": {
        "time": "2026-02-13T13:15:00-05:00",
        "minutes": 12,
        "leave_in_minutes": 7,
        "source": "schedule",
        "trip_id": null
      },
      "alternatives": [
        {
          "time": "2026-02-13T13:45:00-05:00",
          "minutes": 42,
          "leave_in_minutes": 37,
          "source": "schedule",
          "trip_id": null
        }
      ]
    }
  ]
})";

static const char* BOARD_STALE = R"({
  "as_of": "2026-02-13T12:30:00-05:00",
  "items": [
    {
      "key": "route_1_inbound",
      "label": "Route 1 - Harvard Sq",
      "route_id": "1",
      "stop_id": "place-harsq",
      "direction_id": 0,
      "walk_minutes": 4,
      "status": "stale",
      "arrival": {
        "time": "2026-02-13T12:41:00-05:00",
        "minutes": 6,
        "leave_in_minutes": 2,
        "source": "realtime",
        "trip_id": "trip-001"
      },
      "alternatives": [
        {
          "time": "2026-02-13T12:53:00-05:00",
          "minutes": 18,
          "leave_in_minutes": 14,
          "source": "realtime",
          "trip_id": "trip-002"
        }
      ],
      "stale_as_of": "2026-02-13T12:30:00-05:00"
    }
  ]
})";

static const char* BOARD_NO_SERVICE = R"({
  "as_of": "2026-02-14T02:15:00-05:00",
  "items": [
    {
      "key": "route_1_inbound",
      "label": "Route 1 - Harvard Sq",
      "route_id": "1",
      "stop_id": "place-harsq",
      "direction_id": 0,
      "walk_minutes": 4,
      "status": "no_service",
      "arrival": null,
      "alternatives": []
    }
  ]
})";

static const char* BOARD_ERROR = R"({
  "as_of": "2026-02-13T12:34:00-05:00",
  "items": [
    {
      "key": "route_1_inbound",
      "label": "Route 1 - Harvard Sq",
      "route_id": "1",
      "stop_id": "place-harsq",
      "direction_id": 0,
      "walk_minutes": 4,
      "status": "error",
      "arrival": null,
      "alternatives": [],
      "error": {
        "code": "mbta_unreachable",
        "message": "Unable to reach MBTA API"
      }
    }
  ]
})";

static const char* BOARD_TWO_ITEMS = R"({
  "as_of": "2026-02-13T12:34:56-05:00",
  "items": [
    {
      "key": "route_1_inbound",
      "label": "Route 1 - Harvard Sq",
      "status": "ok",
      "walk_minutes": 4,
      "arrival": {
        "time": "2026-02-13T12:41:00-05:00",
        "minutes": 6,
        "leave_in_minutes": 2,
        "source": "realtime",
        "trip_id": "trip-001"
      },
      "alternatives": []
    },
    {
      "key": "route_73_outbound",
      "label": "Route 73 - Mt Auburn",
      "status": "stale",
      "walk_minutes": 5,
      "arrival": {
        "time": "2026-02-13T13:15:00-05:00",
        "minutes": 40,
        "leave_in_minutes": 35,
        "source": "schedule",
        "trip_id": null
      },
      "alternatives": [],
      "stale_as_of": "2026-02-13T12:30:00-05:00"
    }
  ]
})";

// ============================================================
// JSON Parsing Tests
// ============================================================

void setUp(void) {}
void tearDown(void) {}

void test_parse_ok_realtime(void) {
    BoardData data;
    TEST_ASSERT_TRUE(parseBoardResponse(BOARD_OK_REALTIME, data));
    TEST_ASSERT_FALSE(data.parse_error);
    TEST_ASSERT_EQUAL(1, data.item_count);
    TEST_ASSERT_EQUAL_STRING("2026-02-13T12:35:00-05:00", data.as_of);

    BoardItem& item = data.items[0];
    TEST_ASSERT_EQUAL_STRING("route_1_inbound", item.key);
    TEST_ASSERT_EQUAL_STRING("Route 1 - Harvard Sq", item.label);
    TEST_ASSERT_TRUE(item.status == Status::Ok);
    TEST_ASSERT_EQUAL(4, item.walk_minutes);

    TEST_ASSERT_TRUE(item.arrival.valid);
    TEST_ASSERT_EQUAL(6, item.arrival.minutes);
    TEST_ASSERT_EQUAL(2, item.arrival.leave_in_minutes);
    TEST_ASSERT_EQUAL_STRING("realtime", item.arrival.source);
    TEST_ASSERT_EQUAL_STRING("trip-001", item.arrival.trip_id);

    TEST_ASSERT_EQUAL(2, item.alt_count);
    TEST_ASSERT_EQUAL(18, item.alternatives[0].minutes);
    TEST_ASSERT_EQUAL(14, item.alternatives[0].leave_in_minutes);
    TEST_ASSERT_EQUAL(30, item.alternatives[1].minutes);
}

void test_parse_ok_schedule(void) {
    BoardData data;
    TEST_ASSERT_TRUE(parseBoardResponse(BOARD_OK_SCHEDULE, data));
    TEST_ASSERT_EQUAL(1, data.item_count);

    BoardItem& item = data.items[0];
    TEST_ASSERT_TRUE(item.status == Status::Ok);
    TEST_ASSERT_EQUAL_STRING("schedule", item.arrival.source);
    TEST_ASSERT_EQUAL(12, item.arrival.minutes);
    TEST_ASSERT_EQUAL(7, item.arrival.leave_in_minutes);
    // trip_id is null in JSON -> empty string
    TEST_ASSERT_EQUAL_STRING("", item.arrival.trip_id);
    TEST_ASSERT_EQUAL(1, item.alt_count);
}

void test_parse_stale(void) {
    BoardData data;
    TEST_ASSERT_TRUE(parseBoardResponse(BOARD_STALE, data));
    TEST_ASSERT_EQUAL(1, data.item_count);

    BoardItem& item = data.items[0];
    TEST_ASSERT_TRUE(item.status == Status::Stale);
    TEST_ASSERT_TRUE(item.arrival.valid);
    TEST_ASSERT_EQUAL(6, item.arrival.minutes);
    TEST_ASSERT_EQUAL_STRING("2026-02-13T12:30:00-05:00", item.stale_as_of);
}

void test_parse_no_service(void) {
    BoardData data;
    TEST_ASSERT_TRUE(parseBoardResponse(BOARD_NO_SERVICE, data));
    TEST_ASSERT_EQUAL(1, data.item_count);

    BoardItem& item = data.items[0];
    TEST_ASSERT_TRUE(item.status == Status::NoService);
    TEST_ASSERT_FALSE(item.arrival.valid);
    TEST_ASSERT_EQUAL(0, item.arrival.minutes);
    TEST_ASSERT_EQUAL(0, item.alt_count);
}

void test_parse_error(void) {
    BoardData data;
    TEST_ASSERT_TRUE(parseBoardResponse(BOARD_ERROR, data));
    TEST_ASSERT_EQUAL(1, data.item_count);

    BoardItem& item = data.items[0];
    TEST_ASSERT_TRUE(item.status == Status::Error);
    TEST_ASSERT_FALSE(item.arrival.valid);
    TEST_ASSERT_EQUAL_STRING("mbta_unreachable", item.error_code);
    TEST_ASSERT_EQUAL_STRING("Unable to reach MBTA API", item.error_message);
}

void test_parse_two_items(void) {
    BoardData data;
    TEST_ASSERT_TRUE(parseBoardResponse(BOARD_TWO_ITEMS, data));
    TEST_ASSERT_EQUAL(2, data.item_count);
    TEST_ASSERT_EQUAL_STRING("2026-02-13T12:34:56-05:00", data.as_of);

    TEST_ASSERT_TRUE(data.items[0].status == Status::Ok);
    TEST_ASSERT_EQUAL(6, data.items[0].arrival.minutes);

    TEST_ASSERT_TRUE(data.items[1].status == Status::Stale);
    TEST_ASSERT_EQUAL(40, data.items[1].arrival.minutes);
    TEST_ASSERT_EQUAL_STRING("2026-02-13T12:30:00-05:00", data.items[1].stale_as_of);
}

void test_parse_malformed_json(void) {
    BoardData data;
    TEST_ASSERT_FALSE(parseBoardResponse("{not valid json", data));
    TEST_ASSERT_TRUE(data.parse_error);
}

void test_parse_missing_items_key(void) {
    BoardData data;
    TEST_ASSERT_FALSE(parseBoardResponse(R"({"as_of": "2026-01-01"})", data));
    TEST_ASSERT_TRUE(data.parse_error);
}

void test_parse_empty_items(void) {
    BoardData data;
    TEST_ASSERT_TRUE(parseBoardResponse(R"({"as_of": "2026-01-01", "items": []})", data));
    TEST_ASSERT_FALSE(data.parse_error);
    TEST_ASSERT_EQUAL(0, data.item_count);
}

void test_parse_unknown_status(void) {
    const char* json = R"({
      "as_of": "2026-01-01",
      "items": [{"key": "x", "status": "banana", "arrival": null, "alternatives": []}]
    })";
    BoardData data;
    TEST_ASSERT_TRUE(parseBoardResponse(json, data));
    TEST_ASSERT_TRUE(data.items[0].status == Status::Unknown);
}

// ============================================================
// Single-item parsing (for /v1/board/{key} responses)
// ============================================================

void test_parse_single_ok(void) {
    const char* json = R"({
      "key": "route_1_inbound",
      "label": "Route 1 - Harvard Sq",
      "status": "ok",
      "walk_minutes": 4,
      "arrival": {
        "minutes": 6,
        "leave_in_minutes": 2,
        "source": "realtime",
        "time": "2026-02-13T12:41:00-05:00",
        "trip_id": "trip-001"
      },
      "alternatives": []
    })";
    BoardItem item;
    TEST_ASSERT_TRUE(parseSingleItem(json, item));
    TEST_ASSERT_TRUE(item.status == Status::Ok);
    TEST_ASSERT_EQUAL(6, item.arrival.minutes);
    TEST_ASSERT_EQUAL_STRING("Route 1 - Harvard Sq", item.label);
}

void test_parse_single_malformed(void) {
    BoardItem item;
    TEST_ASSERT_FALSE(parseSingleItem("not json", item));
}

// ============================================================
// Bar fill math tests
// ============================================================

void test_bar_fill_arriving_now(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, barFill(0, 30));
}

void test_bar_fill_at_max(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, barFill(30, 30));
}

void test_bar_fill_beyond_max(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, barFill(45, 30));
}

void test_bar_fill_midpoint(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, barFill(15, 30));
}

void test_bar_fill_negative_minutes(void) {
    // Bus already passed -- bar should be full
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, barFill(-5, 30));
}

void test_bar_fill_zero_window(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, barFill(10, 0));
}

// ============================================================
// Should-leave threshold tests
// ============================================================

void test_should_leave_below_threshold(void) {
    TEST_ASSERT_TRUE(shouldLeave(5, 6));
}

void test_should_leave_at_threshold(void) {
    TEST_ASSERT_TRUE(shouldLeave(6, 6));
}

void test_should_leave_above_threshold(void) {
    TEST_ASSERT_FALSE(shouldLeave(7, 6));
}

void test_should_leave_zero(void) {
    TEST_ASSERT_TRUE(shouldLeave(0, 6));
}

void test_should_leave_negative(void) {
    // Already too late -- still returns true
    TEST_ASSERT_TRUE(shouldLeave(-2, 6));
}

// ============================================================
// Entry point
// ============================================================

int main(int argc, char** argv) {
    UNITY_BEGIN();

    // JSON parsing -- multi-stop
    RUN_TEST(test_parse_ok_realtime);
    RUN_TEST(test_parse_ok_schedule);
    RUN_TEST(test_parse_stale);
    RUN_TEST(test_parse_no_service);
    RUN_TEST(test_parse_error);
    RUN_TEST(test_parse_two_items);
    RUN_TEST(test_parse_malformed_json);
    RUN_TEST(test_parse_missing_items_key);
    RUN_TEST(test_parse_empty_items);
    RUN_TEST(test_parse_unknown_status);

    // JSON parsing -- single-stop
    RUN_TEST(test_parse_single_ok);
    RUN_TEST(test_parse_single_malformed);

    // Bar fill math
    RUN_TEST(test_bar_fill_arriving_now);
    RUN_TEST(test_bar_fill_at_max);
    RUN_TEST(test_bar_fill_beyond_max);
    RUN_TEST(test_bar_fill_midpoint);
    RUN_TEST(test_bar_fill_negative_minutes);
    RUN_TEST(test_bar_fill_zero_window);

    // Should-leave threshold
    RUN_TEST(test_should_leave_below_threshold);
    RUN_TEST(test_should_leave_at_threshold);
    RUN_TEST(test_should_leave_above_threshold);
    RUN_TEST(test_should_leave_zero);
    RUN_TEST(test_should_leave_negative);

    return UNITY_END();
}
