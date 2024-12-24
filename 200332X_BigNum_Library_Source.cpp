#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <string>

class BigNumber
{

private:
    // Vector to store the digits of the number in reverse order (least significant digit first)
    std::vector<int> number_digits;

public:
    // Default constructor to initialize BigNumber to zero
    BigNumber()
    {
        number_digits.push_back(0);
    }

    // Constructor to initialize BigNumber from a string
    BigNumber(const std::string &number)
    {
        // Resize the vector to fit the number of digits
        number_digits.resize(number.size());
        // Iterate over the string and set the digits in reverse order
        for (int i = 0; i < number.size(); i++)
        {
            number_digits[number.size() - i - 1] = number[i] - '0'; // Convert char to integer
        }
    }

    // Convert the number to string
    std::string to_string() const
    {
        std::ostringstream oss;
        // Print digits in reverse order (most significant digit first)
        bool leading_zero = true;
        for (int i = number_digits.size() - 1; i >= 0; i--)
        {
            if (number_digits[i] == 0 && leading_zero)
                continue; // Skip leading zeros
            leading_zero = false;
            oss << number_digits[i];
        }
        if (leading_zero)
            oss << '0'; // If all digits were zero
        return oss.str();
    }

    // modAdd function to add two BigNumber objects and return the result modulo a given modulus
    BigNumber modAdd(const BigNumber &big_number, const BigNumber &modulus)
    {
        BigNumber result = *this + big_number;
        return result % modulus;
    }

    // modMultiplication function to multiply two BigNumber objects and return the result modulo a given modulus
    BigNumber modMultiplication(const BigNumber &big_number, const BigNumber &modulus)
    {
        BigNumber result = *this * big_number;
        return result % modulus;
    }

    // modInverse function to find the modular inverse of a BigNumber object with respect to a given modulus
    // Uses the Extended Euclidean Algorithm
    BigNumber modInverse(const BigNumber &modulus)
    {
        BigNumber a = *this;
        BigNumber m = modulus;
        BigNumber m0 = m;
        BigNumber t;
        BigNumber q;
        BigNumber x0("0");
        BigNumber x1("1");

        if (m == BigNumber("1"))
        {
            return BigNumber("0");
        }

        while (a > BigNumber("1"))
        {
            // q is quotient
            q = a / m;

            t = m;

            // m is remainder now, process same as Euclid's algo
            m = a % m;
            a = t;

            t = x0;

            x0 = x1 - q * x0;

            x1 = t;
        }

        // Make x1 positive
        if (x1 < BigNumber("0"))
        {
            x1 = x1 + m0;
        }

        return x1;
    }

    // Add two BigNumber objects
    BigNumber operator+(const BigNumber &big_number) const
    {
        BigNumber result("0"); // Initialize result to zero
        int carry = 0;         // Initialize carry to zero
        int max_size = std::max(number_digits.size(), big_number.number_digits.size());
        // Iterate over the digits of both numbers
        for (int i = 0; i < max_size || carry; i++)
        {
            if (i == result.number_digits.size())
                result.number_digits.push_back(0); // Ensure enough space
            int sum = carry;
            if (i < number_digits.size())
                sum += number_digits[i]; // Add digit from first number
            if (i < big_number.number_digits.size())
                sum += big_number.number_digits[i]; // Add digit from second number
            result.number_digits[i] = sum % 10;     // Store the digit in the result
            carry = sum / 10;                       // Update the carry
        }
        return result;
    }

    // Subtract two BigNumber objects
    BigNumber operator-(const BigNumber &big_number) const
    {
        BigNumber result = *this; // Copy of the number to perform operations on
        int borrow = 0;           // Initialize borrow to zero

        // Iterate over the digits of the second number
        for (int i = 0; i < big_number.number_digits.size() || borrow; i++)
        {
            result.number_digits[i] -= borrow; // Subtract the borrow
            if (i < big_number.number_digits.size())
                result.number_digits[i] -= big_number.number_digits[i]; // Subtract the digit from the second number
            if (result.number_digits[i] < 0)                            // If the digit is negative
            {
                result.number_digits[i] += 10; // Add 10 to the digit
                borrow = 1;                    // Set borrow to 1
            }
            else
            {
                borrow = 0; // Set borrow to 0
            }
        }
        // Remove leading zeros
        while (result.number_digits.size() > 1 && result.number_digits.back() == 0)
        {
            result.number_digits.pop_back();
        }
        return result;
    }

    // Overload for multiplication of BigNumber and int
    BigNumber operator*(const BigNumber &big_number) const
    {
        BigNumber result("0"); // Initialize result to zero
        result.number_digits.resize(number_digits.size() + big_number.number_digits.size(), 0);

        // Iterate over the digits of the first number
        for (int i = 0; i < number_digits.size(); i++)
        {
            int carry = 0; // Initialize carry to zero
            // Iterate over the digits of the second number
            for (int j = 0; j < big_number.number_digits.size() || carry; j++)
            {
                long long current = result.number_digits[i + j] + number_digits[i] * 1LL * (j < big_number.number_digits.size() ? big_number.number_digits[j] : 0) + carry;
                result.number_digits[i + j] = current % 10; // Store the digit in the result
                carry = current / 10;                       // Update the carry
            }
        }
        // Remove leading zeros
        while (result.number_digits.size() > 1 && result.number_digits.back() == 0)
        {
            result.number_digits.pop_back();
        }
        return result;
    }

    // Overload for division of BigNumber
    BigNumber operator/(const BigNumber &big_number) const
    {
        BigNumber quotient("0");  // Initialize quotient to zero
        BigNumber remainder("0"); // Initialize remainder to zero

        // Iterate over the digits of the first number
        for (int i = number_digits.size() - 1; i >= 0; i--)
        {
            // Bring down the next digit
            remainder = remainder * BigNumber("10") + BigNumber(std::to_string(number_digits[i]));
            // Determine how many times the divisor fits into the current remainder
            BigNumber count("0");
            while (remainder >= big_number)
            {
                remainder = remainder - big_number;
                count = count + BigNumber("1");
            }
            quotient.number_digits.push_back(count.number_digits[0]); // Push the result into quotient
        }
        // Reverse the digits of the quotient
        std::reverse(quotient.number_digits.begin(), quotient.number_digits.end());
        return quotient;
    }

    // Modulus operator
    BigNumber operator%(const BigNumber &modulus) const
    {
        BigNumber dividend = *this;  // Copy of the number to perform operations on
        BigNumber divisor = modulus; // Copy of the modulus

        // If the dividend is smaller than the divisor, return the dividend as the remainder
        if (dividend < divisor)
        {
            return dividend;
        }

        BigNumber remainder("0"); // Remainder starts at 0

        // Long division algorithm
        for (int i = dividend.number_digits.size() - 1; i >= 0; --i)
        {
            // Bring down the next digit
            remainder = remainder * BigNumber("10") + BigNumber(std::to_string(dividend.number_digits[i]));

            // Determine how many times the divisor fits into the current remainder
            BigNumber count("0");
            while (remainder >= divisor)
            {
                remainder = remainder - divisor;
                count = count + BigNumber("1");
            }
        }

        return remainder;
    }

    // Less than operator
    bool operator<(const BigNumber &other) const
    {
        if (number_digits.size() != other.number_digits.size())
        {
            return number_digits.size() < other.number_digits.size();
        }
        for (int i = number_digits.size() - 1; i >= 0; i--)
        {
            if (number_digits[i] != other.number_digits[i])
            {
                return number_digits[i] < other.number_digits[i];
            }
        }
        return false; // Equal
    }

    // Greater than operator
    bool operator>(const BigNumber &other) const
    {
        return other < *this;
    }

    // Greater than or equal to operator
    bool operator>=(const BigNumber &other) const
    {
        return !(*this < other);
    }

    // Less than or equal to operator
    bool operator<=(const BigNumber &other) const
    {
        return !(*this > other);
    }

    // Equal to operator
    bool operator==(const BigNumber &other) const
    {
        return !(*this < other) && !(other < *this);
    }

    // Not equal to operator
    bool operator!=(const BigNumber &other) const
    {
        return *this < other || *this > other;
    }
};

int main()
{
    // Create two BigNumber objects with strings
    BigNumber num1("10437967034550369841185788912595856296703254802081129894622611372354751621655182067122922816461741030830055");
    BigNumber num2("87417973915802325716881249516642714001323911131968838890174770674722881348278688527052102460032139990458100");
    BigNumber num3("0");
    BigNumber num4("42694698439494698461");
    BigNumber num5("69716961494698416846841652164684116946194264624698446846246861984269468426849842946292461871487614");
    BigNumber num6("681469846984698496841694191269842646819184694646942646914926984616946194262468984691691691942464614642414924292491494194942941941941912492494919419494991249494229849494168148414514684161414624649841681469824692446464261642694964646468469842648644444468416941694942642");
    BigNumber num7("104379670345503698411857889125958562967032548020811298946226113723547516216551820671229228164617410308300558741797391580232571688124951664271400132391113196883889017477067472288134827868852705210246003213999045810042694698439494698461");
    BigNumber num8("6419664");
    BigNumber num9("2649161");

    BigNumber modulus_512("13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084096");
    BigNumber modulus_1024("179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216");
    BigNumber modulus_2048("32317006071311007300714876688669951960444102669715484032130345427524655138867890893197201411522913463688717960921898019494119559150490921095088152386448283120630877367300996091750197750389652106796057638384067568276792218642619756161838094338476170470581645852036305042887575891541065808607552399123930385521914333389668342420684974786564569494856176035326322058077805659331026192708460314150258592864177116725943603718461857357598351152301645904403697613233287231227125684710820209725157101726931323469678542580656697935045997268352998638215525166389437335543602135433229604645318478604952148193555853611059596230656");

    // Print the numbers
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num1.to_string() << std::endl;
    std::cout << "Num2: " << num2.to_string() << std::endl;
    std::cout << "Modulus: 512bits" << std::endl;
    BigNumber additionModulus1 = num1.modAdd(num2, modulus_512);
    std::cout << "Modulus Addition: " << additionModulus1.to_string() << std::endl;

    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num1.to_string() << std::endl;
    std::cout << "Num2: " << num3.to_string() << std::endl;
    std::cout << "Modulus: 512bits" << std::endl;
    BigNumber additionModulus2 = num1.modAdd(num3, modulus_512);
    std::cout << "Modulus Addition: " << additionModulus2.to_string() << std::endl;

    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num6.to_string() << std::endl;
    std::cout << "Num2: " << num7.to_string() << std::endl;
    std::cout << "Modulus: 1024bits" << std::endl;
    BigNumber additionModulus3 = num6.modAdd(num7, modulus_1024);
    std::cout << "Modulus Addition: " << additionModulus3.to_string() << std::endl;

    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num6.to_string() << std::endl;
    std::cout << "Num2: " << num7.to_string() << std::endl;
    std::cout << "Modulus: 2048bits" << std::endl;
    BigNumber additionModulus4 = num6.modAdd(num7, modulus_2048);
    std::cout << "Modulus Addition: " << additionModulus4.to_string() << std::endl;

    // Test numbers for multiplication
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num1.to_string() << std::endl;
    std::cout << "Num2: " << num2.to_string() << std::endl;
    std::cout << "Modulus: 512bits" << std::endl;
    BigNumber multiplicationModulus1 = num1.modMultiplication(num2, modulus_512);
    std::cout << "Modulus Multiplication: " << multiplicationModulus1.to_string() << std::endl;

    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num1.to_string() << std::endl;
    std::cout << "Num2: " << num3.to_string() << std::endl;
    std::cout << "Modulus: 512bits" << std::endl;
    BigNumber multiplicationModulus2 = num1.modMultiplication(num3, modulus_512);
    std::cout << "Modulus Multiplication: " << multiplicationModulus2.to_string() << std::endl;

    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num6.to_string() << std::endl;
    std::cout << "Num2: " << num7.to_string() << std::endl;
    std::cout << "Modulus: 1024bits" << std::endl;
    BigNumber multiplicationModulus3 = num6.modMultiplication(num7, modulus_1024);
    std::cout << "Modulus Multiplication: " << multiplicationModulus3.to_string() << std::endl;

    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num6.to_string() << std::endl;
    std::cout << "Num2: " << num7.to_string() << std::endl;
    std::cout << "Modulus: 2048bits" << std::endl;
    BigNumber multiplicationModulus4 = num6.modMultiplication(num7, modulus_2048);
    std::cout << "Modulus Multiplication: " << multiplicationModulus4.to_string() << std::endl;

    // Test numbers for inverse

    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num9.to_string() << std::endl;
    std::cout << "Modulus: 512bits" << std::endl;
    BigNumber inverseModulus2 = num9.modInverse(modulus_512);
    std::cout << "Modulus Inverse: " << inverseModulus2.to_string() << std::endl;

    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num7.to_string() << std::endl;
    std::cout << "Modulus: 1024bits" << std::endl;
    BigNumber inverseModulus3 = num7.modInverse(modulus_1024);
    std::cout << "Modulus Inverse: " << inverseModulus3.to_string() << std::endl;

    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num7.to_string() << std::endl;
    std::cout << "Modulus: 2048bits" << std::endl;
    BigNumber inverseModulus4 = num7.modInverse(modulus_2048);
    std::cout << "Modulus Inverse: " << inverseModulus4.to_string() << std::endl;

    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "Num1: " << num1.to_string() << std::endl;
    std::cout << "Modulus: 2048bits" << std::endl;
    BigNumber inverseModulus5 = num1.modInverse(modulus_2048);
    std::cout << "Modulus Inverse: " << inverseModulus5.to_string() << std::endl;

    return 0;
}
