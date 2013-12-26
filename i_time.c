#include "i_time.h"

int diff_tm(const struct tm *tm2, const struct tm *tm1, int dt_type)
{
    int i;

    int _yy = 0;
    _yy = tm2->tm_year - tm1->tm_year;

    if (_yy < 0) {
        return ((i = diff_tm(tm1, tm2, dt_type)) == INT_MAX ? i : (0 - i));
    }

    if (dt_type == DATE_TYPE_YEAR) {
        return _yy;
    }

    int _mm = 0;
    _mm = _yy * 12 + tm2->tm_mon - tm1->tm_mon;

    if (dt_type == DATE_TYPE_MON) {
        return _mm;
    }

    int leap;
    int _dd = 0;

    for (i = tm1->tm_year; i < tm2->tm_year; ++i) {
        _dd += (leapyear(i + 1900) ? 366 : 365);
    }

    leap = leapyear(tm2->tm_year + 1900);
    for (i = 0; i < tm2->tm_mon; ++i) {
        _dd += _mon_mday[leap][i];
    }
    _dd += tm2->tm_mday;

    leap = leapyear(tm1->tm_year + 1900);
    for (i = 0; i < tm1->tm_mon; ++i) {
        _dd -= _mon_mday[leap][i];
    }
    _dd -= tm1->tm_mday;

    if (dt_type == DATE_TYPE_DAY) {
        return _dd;
    }

    int _hh = 0;
    _hh = _dd * 24 + tm2->tm_hour - tm1->tm_hour;

    if (dt_type == DATE_TYPE_HOUR) {
        return _hh;
    }

    int _mi = 0;
    _mi = _hh * 60 + tm2->tm_min - tm1->tm_min;

    if (dt_type == DATE_TYPE_MIN) {
        if (_yy > MIN_MAX_YEAR) {
            return DIFF_ERROR;
        }
        return _mi;
    }

    int _ss = 0;
    _ss = _mi * 60 + tm2->tm_sec - tm1->tm_sec;

    if (dt_type == DATE_TYPE_SEC) {
        if (_yy > SEC_MAX_YEAR) {
            return DIFF_ERROR;
        }
        return _ss;
    }

    return DIFF_ERROR;
}

void modi_tm(struct tm *tm2, const struct tm *tm1, int dt_off, int dt_type)
{
    copy_tm(tm2, tm1);

    if (dt_off == 0) {
        return;
    }

    if (dt_type == DATE_TYPE_YEAR) {
        tm2->tm_year = tm1->tm_year + dt_off;
        return;
    }

    if (dt_type == DATE_TYPE_MON) {
        int _mm = 0;
        _mm = tm1->tm_mon + dt_off;

        tm2->tm_year = tm1->tm_year + _mm / 12 - (_mm < 0);
        tm2->tm_mon = (_mm + 12) % 12;
        return;
    }

    if (dt_type == DATE_TYPE_DAY) {
        int leap;
        int i;

        int _dd = 0;
        _dd = tm1->tm_mday + dt_off;

        if (_dd > 0 && _dd <= _mon_mday[leapyear(tm1->tm_year)][tm1->tm_mon]) {
            tm2->tm_mday = _dd;
            return;
        } else {
            leap = leapyear(tm1->tm_year);
            for (i = 0; i < tm1->tm_mon; ++i) {
                _dd += _mon_mday[leap][i];
            }
            if (_dd > 0) {
                for (i = tm1->tm_year + 1900; _dd > (leapyear(i) ? 366 : 365); ++i) {
                    _dd -= (leapyear(i) ? 366 : 365);
                }
            } else {
                for (i = tm1->tm_year + 1900; _dd <= 0; --i) {
                    _dd += (leapyear(i - 1) ? 366 : 365);
                }
            }
            tm2->tm_year = i - 1900;
            leap = leapyear(tm2->tm_year);
            for (i = 0; _dd > _mon_mday[leap][i]; ++i) {
                _dd -= _mon_mday[leap][i];
            }
            tm2->tm_mon = i;
            tm2->tm_mday = _dd;
            return;
        }
    }

    struct tm tm3;

    if (dt_type == DATE_TYPE_HOUR) {
        int _hh = 0;
        _hh = tm1->tm_hour + dt_off;

        if (_hh >= 0 && _hh < 24) {
            tm2->tm_hour = _hh;
            return;
        } else {
            tm2->tm_hour = _hh % 24;
            copy_tm(&tm3, tm2);
            return modi_tm(tm2, &tm3, _hh / 24 - (_hh < 0), DATE_TYPE_DAY);
        }
    }

    if (dt_type == DATE_TYPE_MIN) {
        int _mi = 0;
        _mi = tm1->tm_min + dt_off;

        if (_mi >=0 && _mi < 60) {
            tm2->tm_min = _mi;
            return;
        } else {
            tm2->tm_min = _mi % 60;
            copy_tm(&tm3, tm2);
            return modi_tm(tm2, &tm3, _mi / 60 - (_mi < 0), DATE_TYPE_HOUR);
        }
    }

    if (dt_type == DATE_TYPE_SEC) {
        int _ss = 0;
        _ss = tm1->tm_sec + dt_off;

        if (_ss >= 0 && _ss < 60) {
            tm2->tm_sec = _ss;
            return;
        } else {
            tm2->tm_sec = _ss % 60;
            copy_tm(&tm3, tm2);
            return modi_tm(tm2, &tm3, _ss / 60 - (_ss < 0), DATE_TYPE_MIN);
        }
    }
}

void copy_tm(struct tm *tm2, const struct tm *tm1) {
    tm2->tm_sec  = tm1->tm_sec;
    tm2->tm_min  = tm1->tm_min;
    tm2->tm_hour = tm1->tm_hour;
    tm2->tm_mday = tm1->tm_mday;
    tm2->tm_mon  = tm1->tm_mon;
    tm2->tm_year = tm1->tm_year;
    tm2->tm_yday = tm1->tm_yday;
    tm2->tm_isdst = tm1->tm_isdst;
}

int get_date(char *date)
{
    time_t now;
    struct tm *now_tm;
    time(&now);
    now_tm = localtime(&now);
    return strftime(date, TIME_LEN, "%Y%m%d", now_tm);
}

int get_datetime(char *datetime)
{
    time_t now;
    struct tm *now_tm;
    time(&now);
    now_tm = localtime(&now);
    return strftime(datetime, TIME_LEN, "%F %T", now_tm);
}
