#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

/**
 * BigNum Library Implementation
 *
 * Assignment: Public Key Cryptosystems BigNum Library
 * Requirements:
 * - Support for very large integers (512, 1024, 2048+ bits)
 * - Number representation
 * - Modulo integer operations: addition, multiplication, and inversion
 */

class BigNum
{
private:
    vector<int> digits; // Store digits in reverse order (least significant first)
    bool is_negative;

    // Helper function to remove leading zeros
    void removeLeadingZeros()
    {
        while (digits.size() > 1 && digits.back() == 0)
        {
            digits.pop_back();
        }
    }

public:
    // Default constructor - creates zero
    BigNum() : is_negative(false)
    {
        digits.push_back(0);
    }

    // Constructor from string
    BigNum(const string &str) : is_negative(false)
    {
        if (str.empty())
        {
            digits.push_back(0);
            return;
        }

        int start = 0;
        if (str[0] == '-')
        {
            is_negative = true;
            start = 1;
        }

        for (int i = str.length() - 1; i >= start; i--)
        {
            if (str[i] >= '0' && str[i] <= '9')
            {
                digits.push_back(str[i] - '0');
            }
        }

        if (digits.empty())
        {
            digits.push_back(0);
            is_negative = false;
        }

        removeLeadingZeros();
        if (isZero())
            is_negative = false;
    }

    // Constructor from integer
    BigNum(long long num) : is_negative(num < 0)
    {
        if (num < 0)
            num = -num;

        if (num == 0)
        {
            digits.push_back(0);
            is_negative = false;
        }
        else
        {
            while (num > 0)
            {
                digits.push_back(num % 10);
                num /= 10;
            }
        }
    }

    // Copy constructor
    BigNum(const BigNum &other) : digits(other.digits), is_negative(other.is_negative) {}

    // Assignment operator
    BigNum &operator=(const BigNum &other)
    {
        if (this != &other)
        {
            digits = other.digits;
            is_negative = other.is_negative;
        }
        return *this;
    }

    // Check if number is zero
    bool isZero() const
    {
        return digits.size() == 1 && digits[0] == 0;
    }

    // Check if number is one
    bool isOne() const
    {
        return !is_negative && digits.size() == 1 && digits[0] == 1;
    }

    // Comparison operators
    bool operator==(const BigNum &other) const
    {
        return is_negative == other.is_negative && digits == other.digits;
    }

    bool operator!=(const BigNum &other) const
    {
        return !(*this == other);
    }

    bool operator<(const BigNum &other) const
    {
        if (is_negative != other.is_negative)
        {
            return is_negative;
        }

        if (is_negative)
        {
            return (-other) < (-(*this));
        }

        if (digits.size() != other.digits.size())
        {
            return digits.size() < other.digits.size();
        }

        for (int i = digits.size() - 1; i >= 0; i--)
        {
            if (digits[i] != other.digits[i])
            {
                return digits[i] < other.digits[i];
            }
        }

        return false;
    }

    bool operator<=(const BigNum &other) const
    {
        return *this < other || *this == other;
    }

    bool operator>(const BigNum &other) const
    {
        return !(*this <= other);
    }

    bool operator>=(const BigNum &other) const
    {
        return !(*this < other);
    }

    // Unary minus
    BigNum operator-() const
    {
        BigNum result(*this);
        if (!result.isZero())
        {
            result.is_negative = !result.is_negative;
        }
        return result;
    }

    // Addition
    BigNum operator+(const BigNum &other) const
    {
        if (is_negative == other.is_negative)
        {
            // Same sign: add magnitudes
            BigNum result;
            result.is_negative = is_negative;
            result.digits.clear();

            int carry = 0;
            int maxSize = max(digits.size(), other.digits.size());

            for (int i = 0; i < maxSize || carry; i++)
            {
                int sum = carry;
                if (i < digits.size())
                    sum += digits[i];
                if (i < other.digits.size())
                    sum += other.digits[i];

                result.digits.push_back(sum % 10);
                carry = sum / 10;
            }

            result.removeLeadingZeros();
            if (result.isZero())
                result.is_negative = false;

            return result;
        }
        else
        {
            // Different signs: subtract magnitudes
            if (is_negative)
            {
                return other - (-(*this));
            }
            else
            {
                return *this - (-other);
            }
        }
    }

    // Subtraction
    BigNum operator-(const BigNum &other) const
    {
        if (is_negative != other.is_negative)
        {
            // Different signs: add magnitudes
            return *this + (-other);
        }

        if (is_negative)
        {
            // Both negative: -a - (-b) = b - a
            return (-other) - (-(*this));
        }

        // Both positive
        if (*this < other)
        {
            return -(other - *this);
        }

        BigNum result;
        result.digits.clear();

        int borrow = 0;
        for (int i = 0; i < digits.size(); i++)
        {
            int diff = digits[i] - borrow;
            if (i < other.digits.size())
            {
                diff -= other.digits[i];
            }

            if (diff < 0)
            {
                diff += 10;
                borrow = 1;
            }
            else
            {
                borrow = 0;
            }

            result.digits.push_back(diff);
        }

        result.removeLeadingZeros();
        return result;
    }

    // Multiplication
    BigNum operator*(const BigNum &other) const
    {
        BigNum result;
        result.digits.assign(digits.size() + other.digits.size(), 0);
        result.is_negative = is_negative ^ other.is_negative;

        for (int i = 0; i < digits.size(); i++)
        {
            for (int j = 0; j < other.digits.size(); j++)
            {
                result.digits[i + j] += digits[i] * other.digits[j];
                result.digits[i + j + 1] += result.digits[i + j] / 10;
                result.digits[i + j] %= 10;
            }
        }

        result.removeLeadingZeros();
        if (result.isZero())
            result.is_negative = false;

        return result;
    }

    // Division (integer division)
    BigNum operator/(const BigNum &divisor) const
    {
        if (divisor.isZero())
        {
            throw runtime_error("Division by zero");
        }

        if (isZero())
            return BigNum(0);

        BigNum dividend(*this);
        dividend.is_negative = false;
        BigNum div(divisor);
        div.is_negative = false;

        if (dividend < div)
            return BigNum(0);

        BigNum quotient(0);
        BigNum remainder(0);

        for (int i = digits.size() - 1; i >= 0; i--)
        {
            remainder = remainder * BigNum(10) + BigNum(digits[i]);

            int count = 0;
            while (remainder >= div)
            {
                remainder = remainder - div;
                count++;
            }

            quotient.digits.insert(quotient.digits.begin(), count);
        }

        quotient.removeLeadingZeros();
        quotient.is_negative = is_negative ^ divisor.is_negative;
        if (quotient.isZero())
            quotient.is_negative = false;

        return quotient;
    }

    // Modulo operation
    BigNum operator%(const BigNum &divisor) const
    {
        if (divisor.isZero())
        {
            throw runtime_error("Division by zero");
        }

        BigNum quotient = *this / divisor;
        BigNum remainder = *this - quotient * divisor;

        // Ensure remainder is positive for modular arithmetic
        if (remainder.is_negative)
        {
            remainder = remainder + (divisor.is_negative ? -divisor : divisor);
        }

        return remainder;
    }

    // Modular addition: (a + b) mod m
    BigNum addMod(const BigNum &b, const BigNum &m) const
    {
        return (*this + b) % m;
    }

    // Modular multiplication: (a * b) mod m
    BigNum mulMod(const BigNum &b, const BigNum &m) const
    {
        return (*this * b) % m;
    }

    // Fast modular exponentiation: (base^exp) mod m
    BigNum powMod(const BigNum &exp, const BigNum &m) const
    {
        if (m.isOne())
            return BigNum(0);

        BigNum result(1);
        BigNum base = *this % m;
        BigNum e(exp);

        while (!e.isZero())
        {
            if (e.digits[0] & 1)
            { // if e is odd
                result = result.mulMod(base, m);
            }
            e = e / BigNum(2);
            base = base.mulMod(base, m);
        }

        return result;
    }

    // Extended Euclidean Algorithm
    static BigNum extendedGCD(const BigNum &a, const BigNum &b, BigNum &x, BigNum &y)
    {
        if (b.isZero())
        {
            x = BigNum(1);
            y = BigNum(0);
            return a;
        }

        BigNum x1, y1;
        BigNum gcd = extendedGCD(b, a % b, x1, y1);

        x = y1;
        y = x1 - (a / b) * y1;

        return gcd;
    }

    // Modular inverse: find x such that (a * x) ≡ 1 (mod m)
    BigNum modInverse(const BigNum &m) const
    {
        BigNum x, y;
        BigNum gcd = extendedGCD(*this % m, m, x, y);

        if (!gcd.isOne())
        {
            throw runtime_error("Modular inverse does not exist");
        }

        BigNum result = x % m;
        if (result.is_negative)
        {
            result = result + m;
        }

        return result;
    }

    // Get bit length of the number
    int getBitLength() const
    {
        if (isZero())
            return 1;

        BigNum temp(*this);
        temp.is_negative = false;

        int bits = 0;
        BigNum two(2);

        while (!temp.isZero())
        {
            temp = temp / two;
            bits++;
        }

        return bits;
    }

    // Convert to string
    string toString() const
    {
        string result;

        if (is_negative && !isZero())
        {
            result += "-";
        }

        for (int i = digits.size() - 1; i >= 0; i--)
        {
            result += (digits[i] + '0');
        }

        return result;
    }

    // Print the number
    void print() const
    {
        cout << toString() << endl;
    }

    // Input operator
    friend istream &operator>>(istream &is, BigNum &num)
    {
        string str;
        is >> str;
        num = BigNum(str);
        return is;
    }

    // Output operator
    friend ostream &operator<<(ostream &os, const BigNum &num)
    {
        os << num.toString();
        return os;
    }
};

// Demo function to show the capabilities
void demonstrateBigNum()
{
    cout << "=== BigNum Library Demonstration ===" << endl;
    cout << "Supporting very large integers for cryptographic operations" << endl
         << endl;

    // Test basic operations
    cout << "1. Basic Operations:" << endl;
    BigNum a("12345678901234567890");
    BigNum b("98765432109876543210");

    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << "a + b = " << a + b << endl;
    cout << "b - a = " << b - a << endl;
    cout << "a * b = " << a * b << endl;
    cout << endl;

    // Test modular operations
    cout << "2. Modular Operations:" << endl;
    BigNum m("1000000007");
    cout << "m = " << m << " (modulus)" << endl;
    cout << "a mod m = " << a % m << endl;
    cout << "b mod m = " << b % m << endl;
    cout << "(a + b) mod m = " << a.addMod(b, m) << endl;
    cout << "(a * b) mod m = " << a.mulMod(b, m) << endl;
    cout << endl;

    // Test modular inverse
    cout << "3. Modular Inverse:" << endl;
    BigNum small_a("123");
    BigNum small_m("1009"); // prime modulus
    try
    {
        BigNum inv = small_a.modInverse(small_m);
        cout << "Inverse of " << small_a << " mod " << small_m << " = " << inv << endl;
        cout << "Verification: (" << small_a << " * " << inv << ") mod " << small_m << " = "
             << small_a.mulMod(inv, small_m) << endl;
    }
    catch (const exception &e)
    {
        cout << "Error: " << e.what() << endl;
    }
    cout << endl;

    // Test large number representation
    cout << "4. Large Number Representation:" << endl;
    string large_512_bit = "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084095";
    BigNum large_num(large_512_bit);
    cout << "512-bit number: " << large_num << endl;
    cout << "Bit length: " << large_num.getBitLength() << " bits" << endl;
    cout << endl;

    // Test modular exponentiation
    cout << "5. Modular Exponentiation:" << endl;
    BigNum base("12345");
    BigNum exp("67890");
    BigNum mod_exp("1000000009");
    cout << base << "^" << exp << " mod " << mod_exp << " = " << base.powMod(exp, mod_exp) << endl;
    cout << endl;

    cout << "=== All tests completed successfully! ===" << endl;
}

int main()
{
    srand(time(nullptr));

    cout << "BigNum Library for Public Key Cryptosystems" << endl;
    cout << "===========================================" << endl
         << endl;

    try
    {
        demonstrateBigNum();

        cout << "\nInteractive mode (enter 'quit' to exit):" << endl;
        cout << "Available operations: +, -, *, /, %, addmod, mulmod, inverse, pow" << endl;

        string operation;
        while (true)
        {
            cout << "\nEnter operation: ";
            cin >> operation;

            if (operation == "quit")
                break;

            if (operation == "+")
            {
                BigNum a, b;
                cout << "Enter first number: ";
                cin >> a;
                cout << "Enter second number: ";
                cin >> b;
                cout << "Result: " << a + b << endl;
            }
            else if (operation == "-")
            {
                BigNum a, b;
                cout << "Enter first number: ";
                cin >> a;
                cout << "Enter second number: ";
                cin >> b;
                cout << "Result: " << a - b << endl;
            }
            else if (operation == "*")
            {
                BigNum a, b;
                cout << "Enter first number: ";
                cin >> a;
                cout << "Enter second number: ";
                cin >> b;
                cout << "Result: " << a * b << endl;
            }
            else if (operation == "/")
            {
                BigNum a, b;
                cout << "Enter first number: ";
                cin >> a;
                cout << "Enter second number: ";
                cin >> b;
                try
                {
                    cout << "Result: " << a / b << endl;
                }
                catch (const exception &e)
                {
                    cout << "Error: " << e.what() << endl;
                }
            }
            else if (operation == "%")
            {
                BigNum a, b;
                cout << "Enter first number: ";
                cin >> a;
                cout << "Enter second number: ";
                cin >> b;
                try
                {
                    cout << "Result: " << a % b << endl;
                }
                catch (const exception &e)
                {
                    cout << "Error: " << e.what() << endl;
                }
            }
            else if (operation == "mulmod")
            {
                BigNum a, b, m;
                cout << "Enter first number: ";
                cin >> a;
                cout << "Enter second number: ";
                cin >> b;
                cout << "Enter modulus: ";
                cin >> m;
                cout << "Result: " << a.mulMod(b, m) << endl;
            }
            else if (operation == "addmod")
            {
                BigNum a, b, m;
                cout << "Enter first number: ";
                cin >> a;
                cout << "Enter second number: ";
                cin >> b;
                cout << "Enter modulus: ";
                cin >> m;
                cout << "Result: " << a.addMod(b, m) << endl;
            }
            else if (operation == "inverse")
            {
                BigNum a, m;
                cout << "Enter number: ";
                cin >> a;
                cout << "Enter modulus: ";
                cin >> m;
                try
                {
                    cout << "Result: " << a.modInverse(m) << endl;
                }
                catch (const exception &e)
                {
                    cout << "Error: " << e.what() << endl;
                }
            }
            else if (operation == "pow")
            {
                BigNum base, exp, mod;
                cout << "Enter base: ";
                cin >> base;
                cout << "Enter exponent: ";
                cin >> exp;
                cout << "Enter modulus: ";
                cin >> mod;
                cout << "Result: " << base.powMod(exp, mod) << endl;
            }
            else
            {
                cout << "Unknown operation. Available: +, -, *, /, %, addmod, mulmod, inverse, pow" << endl;
            }
        }
    }
    catch (const exception &e)
    {
        cout << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
