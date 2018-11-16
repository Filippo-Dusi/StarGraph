/*
    CONVERTER
*/

//This file is included only once in a single compilation
#pragma once

//The functions declared in the 'MathFunctions' class are based on those which are located in 'cmath'
#include <cmath>

//Class contatining the 'power_of()' and 'log_base_10()' functions, which are used to calculate relative luminosity from absolute magnitude et vice versa
class MathFunctions
{
public:
    //Returns 'base' to the power of 'exponent'
    static double power_of(double base, double exponent)
    {
        return std::pow(base, exponent);
    }

    //Returns the logarithm of 'param' in base 10
    static double log_base_10(double param)
    {
        return std::log10(param);
    }
};

//Class containing all the functions used for temperature and luminosity conversion
class StarFunctions
{
public:
    //Returns a specific temperature value from the table below: for example 'getSpectralTypeTemperature(4, 2)' means 'G2' and will return 5800
    static int get_spectral_type_temperature(int spectral_class, int spectral_subclass)
    {
        static const int temperature_table[7][10] =
        {
            //'spectral_class = 0' means 'O'
            { 6000, 5700, 5400, 5100, 4800, 4500, 4200, 3900, 3600, 3300 },
            //'spectral_class = 1' means 'B'
            { 3000, 2800, 2600, 2400, 2200, 2000, 1800, 1600, 1400, 1200 },
            //'spectral_class = 2' means 'A'
            { 1000, 975, 950, 925, 900, 875, 850, 825, 800, 775 },
            //'spectral_class = 3' means 'F'
            { 750, 735, 720, 705, 690, 675, 660, 645, 630, 615 },
            //'spectral_class = 4' means 'G'
            { 600, 590, 580, 570, 560, 550, 540, 530, 520, 510 },
            //'spectral_class = 5' means 'K'
            { 500, 485, 470, 455, 440, 425, 410, 395, 380, 365 },
            //'spectral_class = 6' means 'M'
            { 350, 340, 330, 320, 310, 300, 290, 280, 270, 260 }
        };

        return temperature_table[spectral_class][spectral_subclass] * 10;
    }

    //Returns the corresponding temperature for a specific spectral type: for example 'get_temperature(52)' means 'G2' and will return '5800'
    static int get_temperature(int spectral_type_coords)
    {
        return get_spectral_type_temperature((spectral_type_coords - 10) / 10 % 10, (spectral_type_coords + 10) % 10);
    }

    //Returns the corresponding spectral type for a specific temperature: for example 'get_spectral_type(5800)' will return '52', which means 'G2'
    static int get_spectral_type(int temperature)
    {
        int spectral_type = 79;

        for (int i = 0; i < 7; i += 1)
        {
            for (int j = 0; j < 10; j += 1)
            {
                if (temperature >= 2600 && temperature < 60000)
                {
                    if (temperature >= get_spectral_type_temperature(i, j) && temperature < get_spectral_type_temperature(i, j - 1))
                    {
                        spectral_type = (i + 1) * 10 + j;
                    }
                }
                else
                {
                    if (temperature < 2600)
                    {
                        spectral_type = 79;
                    }

                    if (temperature >= 60000)
                    {
                        spectral_type = 10;
                    }
                }
            }
        }

        return spectral_type;
    }

    //Returns the corresponding absolute magniude for a specific relative luminosity: for example 'get_absolute_magnitude(1.0)' will return '4.83'
    static double get_absolute_magnitude(double relative_luminosity)
    {
        return 4.83 - (2.51188643150958 * MathFunctions::log_base_10(relative_luminosity));
    }

    //Returns the corresponding relative luminosity for a specific absolute magniude: for example 'get_relative_luminosity(4.83)' will return '1.0'
    static double get_relative_luminosity(double absolute_magnitude)
    {
        //NOTE: '2.51188643150958' is more accurate than 'MathFunctions::power_f(100, 1.0 / 5)'
        return MathFunctions::power_of(2.51188643150958, 4.83 - absolute_magnitude);
    }
};
