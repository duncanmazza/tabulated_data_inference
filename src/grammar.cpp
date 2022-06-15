/**
 * Definitions for the helper functions used with the parser combinator.
 *
 * @note Refer to https://github.com/orangeduck/mpc for more information on how the grammar definition works.
 * @note The % formats referenced in the inline documentation refer to the boost time formats (see
 *  https://www.boost.org/doc/libs/1_60_0/doc/html/date_time/date_time_io.html)
 * @author Duncan Mazza
 */

#include <grammar.h>

using namespace std;


string mpc_strip_tag(const string &tag) {
    size_t vbar_pos = tag.find('|');
    if (vbar_pos == string::npos) {
        return {tag};
    }
    return tag.substr(0, vbar_pos);
}


void mpc_setup(mpc_parser_t **parser, vector<mpc_parser_t *> &allParsers) {
    // Note that %p is not usable for date/time input, so this will need to be handled as a special case downstream
    // when parsing times
    const char *grammar =
            // Date and time rules:
            "year_4_digit: /[0-9]{4}/;"
            "year_2_digit: /[0-9]{2}/;"
            "day: /[0-2][0-9]|30|31/;"
            "day_single_dig: /[1-9]|[1-9]{2}|30|31|32/;"
            "month_m: /0[0-9]|10|11|12/;"  // Month using the %m format
            "month_b: /Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec/;"  // Month using the %b format
            "month_single_dig: /[1-9]|10|11|12/;"
            "second: /[0-5][0-9]/;"
            "minute: /[0-5][0-9]/;"
            "hour_12: /0[1-9]|10|11|12/;"
            "hour_24: /[0-1][0-9]|20|21|22|23/;"
            "hour_12_single_dig: /[1-9]|10|11|12/;"
            "hour_24_single_dig: /[0-1]|1[0-9]|20|21|22|23/;"
            "fsecond: /\\.[0-9]+/;"  // Fractional second
            "apm: /[AP]M/;"  // AM or PM (must be capitalized)
            "tz_q: /[+-][0-9]{4}/;"  // Time zone using the %q format
            "tz_Q: /[+-][0-9]{2}:[0-9]{2}/;"  // Time zone using the %Q format
            "tz_ZP: /[A-Z]{3}[+-][0-9]{2}/;"  // Time zone using the %ZP format
            "time: <hour_12><minute><second><fsecond>?' '?<apm>(<tz_q> | <tz_Q> | ' '<tz_ZP>)?"
            "    | (<hour_12> | <hour_12_single_dig>)':'<minute>(':'<second><fsecond>?)?' '?<apm>(<tz_q> | <tz_Q> | ' '<tz_ZP>)?"
            "    | <hour_24><minute><second><fsecond>?(' '?<apm>)?(<tz_q> | <tz_Q> | ' '<tz_ZP>)"  // If there is a
            // 6-digit time string, require that it be suffixed by a time zone
            "    | (<hour_24> | <hour_24_single_dig>)':'<minute>(':'<second><fsecond>?)?(' '?<apm>)?(<tz_q> | <tz_Q> | ' '<tz_ZP>)?;"
            "date: <year_4_digit><month_m><day>"
            "    | <month_m><day><year_2_digit>"
            "    | <day><month_m><year_2_digit>"
            "    | (<month_m> | <month_b> | <month_single_dig>)('-' | '/')(<day> | <day_single_dig>)('-' | '/')(<year_4_digit> | <year_2_digit>)"
            "    | <year_4_digit>('-' | '/')(<month_m> | <month_b> | <month_single_dig>)('-' | '/')(<day> | <day_single_dig>);"
            "datetime: <date>(' ' | 'T')?<time>;"

            // All other rules:
            "logical: '0' | '1';"  // Logical value
            "bit_str: /[01]+/;"  // Bit string
            "int: /-?[0-9]+/;"  // Integer
            "float_dec: /-?[0-9]*[.][0-9]+/;"  // Decimal floating point
            "float_exp: /[+-]?[0-9]+([.][0-9]+)?[eE][+-]?[0-9]+/;"  // Scientific notation floating point

            // Put all the rules together here. Though there may be some optimizations to be had about the number of
            // end/start patterns used here, generally speaking, they are necessary (as opposed to grouping some rules
            // together and putting start/end patterns around them).
            "all: /^/<logical>/$/ "
            "   | /^/<bit_str>/$/ "
            "   | /^/("
            "           <datetime> "
            "         | <time> "
            "         | <date>"
            "     )/$/ "
            "   | /^/<int>/$/ "
            "   | /^/("
            "           <float_exp>"
            "         | <float_dec>"
            "     )/$/;";

    // mpc only allows this method of creating a language parser through a variadic function, so manually create every
    // parser needed in the scope.
    mpc_parser_t *p01 = mpc_new("year_4_digit");
    mpc_parser_t *p02 = mpc_new("year_2_digit");
    mpc_parser_t *p03 = mpc_new("day");
    mpc_parser_t *p04 = mpc_new("day_single_dig");
    mpc_parser_t *p05 = mpc_new("month_m");
    mpc_parser_t *p06 = mpc_new("month_b");
    mpc_parser_t *p07 = mpc_new("month_single_dig");
    mpc_parser_t *p08 = mpc_new("second");
    mpc_parser_t *p09 = mpc_new("minute");
    mpc_parser_t *p10 = mpc_new("hour_12");
    mpc_parser_t *p11 = mpc_new("hour_24");
    mpc_parser_t *p12 = mpc_new("hour_12_single_dig");
    mpc_parser_t *p13 = mpc_new("hour_24_single_dig");
    mpc_parser_t *p14 = mpc_new("fsecond");
    mpc_parser_t *p15 = mpc_new("apm");
    mpc_parser_t *p16 = mpc_new("tz_q");
    mpc_parser_t *p17 = mpc_new("tz_Q");
    mpc_parser_t *p18 = mpc_new("tz_ZP");
    mpc_parser_t *p19 = mpc_new("time");
    mpc_parser_t *p20 = mpc_new("date");
    mpc_parser_t *p21 = mpc_new("datetime");
    mpc_parser_t *p22 = mpc_new("logical");
    mpc_parser_t *p23 = mpc_new("bit_str");
    mpc_parser_t *p24 = mpc_new("int");
    mpc_parser_t *p25 = mpc_new("float_dec");
    mpc_parser_t *p26 = mpc_new("float_exp");
    mpc_parser_t *p27 = mpc_new("all");

    mpca_lang(MPCA_LANG_WHITESPACE_SENSITIVE, grammar, p01, p02, p03, p04, p05, p06, p07, p08, p09, p10,
              p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, NULL);
    *parser = p27;

    allParsers.push_back(p01);
    allParsers.push_back(p02);
    allParsers.push_back(p03);
    allParsers.push_back(p04);
    allParsers.push_back(p05);
    allParsers.push_back(p06);
    allParsers.push_back(p07);
    allParsers.push_back(p08);
    allParsers.push_back(p09);
    allParsers.push_back(p10);
    allParsers.push_back(p11);
    allParsers.push_back(p12);
    allParsers.push_back(p13);
    allParsers.push_back(p14);
    allParsers.push_back(p15);
    allParsers.push_back(p16);
    allParsers.push_back(p17);
    allParsers.push_back(p18);
    allParsers.push_back(p19);
    allParsers.push_back(p20);
    allParsers.push_back(p21);
    allParsers.push_back(p22);
    allParsers.push_back(p23);
    allParsers.push_back(p24);
    allParsers.push_back(p25);
    allParsers.push_back(p26);
    allParsers.push_back(p27);
}
