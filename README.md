# Tabulated Data Inference

A library that uses a parser combinator (specifically, [mpc](https://github.com/orangeduck/mpc)) to automatically identify the data types from tabulated data. Its primary advantages are its ability to automatically identify:

- The delimiter (data delimited with `,`, `;`, `\t`, and ` ` (single-space) delimiters are supported) and column names even with arbitrary metadata preceding the actual data (see 2nd file parsing example below).
- Date/time/datetime strings of (nearly) any format with no prior knowledge about the format.

This enumeration from [tabulated_data_inference.h](tabulated_data_inference.h) shows the different data classifications that are supported:

```c
typedef enum {
    FC_0_LOGICAL,  // boolean values
    FC_1_BIT_STR,  // bit string
    FC_2_DT_TIME,  // datetime string
    FC_3_TM_ONLY,  // time string
    FC_4_DT_ONLY,  // date string
    FC_5_INTEGER,  // integer
    FC_6_FLT_DEC,  // floating point (decimal notation)
    FC_7_FLT_EXP,  // floating point (scientific notation)
    FC_8_ARBITRY,  // arbitrary string
} FieldCls;
```

## Examples

### File Parsing Example 1: Scientific instrument data parsing

[Example file input](tests/test_targets/acsm_shortened.csv) (the raw output of an air quality instrument) where the first row contains the column names and the delimiter is a `,`:

| Org       | SO4       | NO3        | NH4        | Chl         | pressureAlarm | airbeamAlarm | RF       | chamberT | Airbeam  | NewStart\_Events | InletPClosed | InletPOpen | InletP  | VapT    | acsm\_local\_time | acsm\_utc\_time |
|:----------|:----------|:-----------|:-----------|:------------|:--------------|:-------------|:---------|:---------|:---------|:-----------------|:-------------|:-----------|:--------|:--------|:------------------|:----------------|
| 3.07175   | -0.170973 | 0.157902   | 0.233803   | 0.0858726   | 0             | 0            | 4.63E-11 | 29.7956  | 6.26E-08 | 0                | 1.33948      | 1.33956    | 1.33954 | 593.185 | 2/9/2022 19:00    | 2/10/2022 0:00  |
| -0.616671 | 0.473042  | 0.147936   | 0.382525   | 0.145136    | 0             | 0            | 4.63E-11 | 29.8115  | 6.29E-08 | 0                | 1.33954      | 1.33978    | 1.33969 | 593.148 | 2/9/2022 19:01    | 2/10/2022 0:01  |
| 2.44781   | 0.221511  | 0.0404302  | -0.449299  | 0.0921875   | 0             | 0            | 4.63E-11 | 29.9742  | 6.26E-08 | 0                | 1.33983      | 1.34023    | 1.34017 | 593.173 | 2/9/2022 19:02    | 2/10/2022 0:02  |
| 1.79036   | 0.833055  | 0.103068   | 0.0850596  | -0.0291055  | 0             | 0            | 4.63E-11 | 30.0265  | 6.30E-08 | 0                | 1.3399       | 1.33983    | 1.3399  | 593.202 | 2/9/2022 19:03    | 2/10/2022 0:03  |
| 1.64374   | 0.337919  | 0.190323   | 0.0358202  | -0.0997532  | 0             | 0            | 4.63E-11 | 30.136   | 6.25E-08 | 0                | 1.3399       | 1.33978    | 1.33984 | 593.137 | 2/9/2022 19:04    | 2/10/2022 0:04  |
| 1.24238   | 0.483508  | 0.122902   | 0.540609   | 0.000719187 | 0             | 0            | 4.63E-11 | 30.2249  | 6.30E-08 | 0                | 1.33997      | 1.34032    | 1.34018 | 593.076 | 2/9/2022 19:05    | 2/10/2022 0:05  |
| 1.76388   | -0.352683 | 0.353515   | -0.0195853 | 0.0112154   | 0             | 0            | 4.63E-11 | 30.2947  | 6.30E-08 | 0                | 1.34017      | 1.33994    | 1.34016 | 593.15  | 2/9/2022 19:06    | 2/10/2022 0:06  |
| -1.02609  | -0.530141 | 0.00655152 | 0.340963   | 0.0961016   | 0             | 0            | 4.63E-11 | 30.3685  | 6.27E-08 | 0                | 1.34054      | 1.34072    | 1.34081 | 593.087 | 2/9/2022 19:07    | 2/10/2022 0:07  |

Example output from running [example_script.cpp](example_script.cpp) with [acsm_shortened.csv](tests/test_targets/acsm_shortened.csv) as the targeted input:

```text
Parsing tests/test_targets/acsm_shortened.csv
Finding delimiter...
Finding fields...
Classifying fields...
Data insights for tests/test_targets/acsm_shortened.csv:
- Org (float_dec)
- SO4 (float_dec)
- NO3 (float_dec)
- NH4 (float_dec)
- Chl (float_dec)
- pressureAlarm (logical)
- airbeamAlarm (logical)
- RF (float_exp)
- chamberT (float_dec)
- Airbeam (float_exp)
- NewStart_Events (logical)
- InletPClosed (float_dec)
- InletPOpen (float_dec)
- InletP (float_dec)
- VapT (float_dec)
- acsm_local_time (datetime)
- acsm_utc_time (datetime)
```

After first identifying the delimiter, the column names and the data types they contain are automatically identified, with distinctions made for floating point numbers in scientific and decimal notation. The flexibility of the parser combinator's grammar allows for date and time strings both with and, in this case, without leading zeros before the day and month.

### File Parsing Example 2 - Engineering Data

[Example file input](tests/test_targets/xf-naca2408-il-50000.csv) (airfoil data from [airfoiltools.com](http://airfoiltools.com/polar/csv?polar=xf-naca2408-il-50000)) where there are several lines of metadata at the beginning of the file:

```text
Xfoil polar. Reynolds number fixed. Mach  number fixed
Polar key,xf-naca2408-il-50000
Airfoil,naca2408-il
Reynolds number,50000
Ncrit,9
Mach,0
Max Cl/Cd,37.4214
Max Cl/Cd alpha,5.5
Url,http://airfoiltools.com/polar/csv?polar=xf-naca2408-il-50000

Alpha,Cl,Cd,Cdp,Cm,Top_Xtr,Bot_Xtr
-9.250,-0.5158,0.11581,0.10876,-0.0005,1.0000,0.2147
-9.000,-0.5047,0.11139,0.10435,0.0000,1.0000,0.2243
-8.750,-0.5244,0.11068,0.10379,-0.0033,1.0000,0.2309
-8.500,-0.5127,0.10634,0.09946,-0.0023,1.0000,0.2449
-8.250,-0.5012,0.10208,0.09521,-0.0015,1.0000,0.2582
... 
(remaining file lines omitted)
```

Example output from running [example_script.cpp](example_script.cpp) with [xf-naca2408-il-50000.csv](tests/test_targets/xf-naca2408-il-50000.csv) as the targeted input:

```text
Parsing tests/test_targets/xf-naca2408-il-50000.csv
Finding delimiter...
Finding fields...
Classifying fields...
Data insights for tests/test_targets/xf-naca2408-il-50000.csv:
 - Alpha (float_dec)
 - Cl (float_dec)
 - Cd (float_dec)
 - Cdp (float_dec)
 - Cm (float_dec)
 - Top_Xtr (float_dec)
 - Bot_Xtr (float_dec)
```

### Examples of Supported Date/Time/Datetime String Identification

| Boost Format                                    | Example                       | Classification |
|:------------------------------------------------|:------------------------------|:---------------|
| `%m-%d-%y`                                      | `04-02-2022`                  | `FC_4_DT_ONLY` |
| `%d%m%y`                                        | `181021`                      | `FC_4_DT_ONLY` |
| `%m-%d-%y`                                      | `04-02-22`                    | `FC_4_DT_ONLY` |
| `%Y-%m-%d` (ISO extended)                       | `2022-04-02`                  | `FC_4_DT_ONLY` |
| `%Y%m%d` (ISO format, date only)                | `20220402`                    | `FC_4_DT_ONLY` |
| N/A (extra character over `%m-%d-%y`)           | `12-34-56789`                 | `FC_8_ARBITRY` |
| N/A (expect 12 hr time when PM is used)         | `13-34-2022 PM`               | `FC_8_ARBITRY` |
| `%H:%M:%S%F %p`                                 | `10:03:22.0023 PM`            | `FC_3_TM_ONLY` |
| `%H:%M:%S%F %p %ZP`                             | `13:03:22.0023 PM MST-07`     | `FC_3_TM_ONLY` |
| `%H%M%S%F%q` (ISO format, time only)            | `131211-0700`                 | `FC_3_TM_ONLY` |
| `%Y-%b-%d %H:%M:%S%F %ZP` (boost default input) | `2005-Oct-15 13:12:11 MST-07` | `FC_2_DT_TIME` |
| `%Y%m%dT%H%M%S%F%q` (ISO format)                | `20051015T131211-0700`        | `FC_2_DT_TIME` |
| N/A (`%m-%d-%y %H$M` sans leading zeros)        | `2/9/2022 0:16`               | `FC_2_DT_TIME` |

Notes:
- The above table was constructed using the unit tests in [test_parsing.cpp](tests/test_parsing.cpp). The parser combinator grammar in [grammar.cpp](src/grammar.cpp) describes completely the various strings that are accepted (both date/time/datetime and otherwise).
  - The grammar specifies, for example, that when faced with a string that can be correctly classified as either a date or a time (e.g., `120402`), the date classification is chosen.
- Refer to [here](https://www.boost.org/doc/libs/1_60_0/doc/html/date_time/date_time_io.html) for the Boost docs on formatting times.

## Compiling and Running

The code is compiled into a static library named `libTabulatedDataInference` using CMake.

- The interface for the library is defined in [tabulated_data_inference.h](tabulated_data_inference.h).
- The grammar relied upon by the parser combinator can be found in [grammar.cpp](src/grammar.cpp)
- Unit tests written using [googletest](https://github.com/google/googletest) can be found in [tests/](tests)

A combination of [Conan](https://conan.io/) and git submodules are used for this library's dependencies:

- Clone this repository with `--recurse-submodules` (or, if you have already cloned, then run `git submodule update --init` from the repository root) to obtain the [googletest](https://github.com/google/googletest) and [mpc](https://github.com/orangeduck/mpc) submodules. 
- Next, create a Python3 virtual environment and pip install `conan`. Activate the virtual environment so that `conan` is a valid command on your path.

Setup, build and run:

```shell
# As recommended on conan's getting started page documentation:
conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default

# Install/build dependencies
mkdir <cmake build dir>
cd <cmake build dir>
conan install .. --build missing

# Build
cmake ..
make *

# Run
cd <repository root>  # The following executables expect the CWD to be the repository root
./<cmake build dir>/example  # Runs inference on files (whose paths are hard-coded) and prints the results
./<cmake build dir>/tests/Google_Tests_run  # Run the unit tests
```

## Future Work

- [ ] The current method by which files are parsed is that they are loaded into memory and every single bit of data is parsed to reach consensus (e.g., the most restrictive data classification is attributed to a given column *after* every row of that column has been parsed). This leaves room for improvement in efficiency (both memory usage and runtime), such as providing an alternative file parsing method that only loads one line of data at a time or a way to specify some number of lines parsed as sufficient for achieving consensus. This would be useful for parsing especially large data files.
- [ ] The current implementation of the grammar leaves room for optimization and expansion of the types of data that can be classified (e.g., there are even more date/time/datetime formats that are not covered by the current grammar).
- [ ] Given that an abstract syntax tree is constructed for every string that is parsed and the date/time/datetime formats are specified in the grammar in a modular way that (mostly) reflects how one would use string formatting flags if converting a datetime object to a string using Boost, it would be possible to traverse these trees to extract the datetime format string that would generate the provided string. There would be caveats, though, as some situations handled by the grammar (such as datetime strings with no leading zeros in front of the day or month) cannot be parsed by Boost (as far as the author is aware).
- [ ] Boost was added as a dependency when building this project out of the expectation that it would provide many useful capabilities. Currently, however, only a single function from Boost is being used. The Boost dependency should either be removed of made better use of. 
- [ ] More test cases in the unit tests.

I welcome contributions, whether to fix bugs, add new features, or improve the documentation.
