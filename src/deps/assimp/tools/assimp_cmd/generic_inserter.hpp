/* Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE. */


#ifndef HEADER_GENERIC_INSERTER_HPP_INCLUDED
#define HEADER_GENERIC_INSERTER_HPP_INCLUDED


#include <ostream>
#include <new> // bad_alloc


template <typename char_type, typename traits_type, typename argument_type>
std::basic_ostream<char_type, traits_type>& generic_inserter(void (*print)(std::basic_ostream<char_type, traits_type>& os, argument_type const& arg), std::basic_ostream<char_type, traits_type>& os, argument_type const& arg)
{
    using namespace ::std;

    ios_base::iostate err = ios_base::goodbit;
    try
    {
        typename basic_ostream<char_type, traits_type>::sentry sentry(os);
        if (sentry)
        {
            print(os, arg);
            err = os.rdstate();
            os.width(0); // Reset width in case the user didn't do it.
        }
    }
    catch (bad_alloc const&)
    {
        err |= ios_base::badbit; // bad_alloc is considered fatal
        ios_base::iostate const exception_mask = os.exceptions();

        // Two cases: 1.) badbit is not set; 2.) badbit is set
        if (((exception_mask & ios_base::failbit) != 0) && // failbit shall throw
            ((exception_mask & ios_base::badbit) == 0))    // badbit shall not throw
        {
            // Do not throw unless failbit is set.
            // If it is set throw ios_base::failure because we don't know what caused the failbit to be set.
            os.setstate(err);
        }
        else if (exception_mask & ios_base::badbit)
        {
            try
            {
                // This will set the badbit and throw ios_base::failure.
                os.setstate(err);
            }
            catch (ios_base::failure const&)
            {
                // Do nothing since we want bad_alloc to be rethrown.
            }
            throw;
        }
        // else: no exception must get out!
    }
    catch (...)
    {
        err |= ios_base::failbit; // Any other exception is considered "only" as a failure.
        ios_base::iostate const exception_mask = os.exceptions();

        // badbit is considered more important
        if (((exception_mask & ios_base::badbit) != 0) && // badbit shall throw
            ((err & ios_base::badbit) != 0))              // badbit is set
        {
            // Throw ios_base::failure because we don't know what caused the badbit to be set.
            os.setstate(err);
        }
        else if ((exception_mask & ios_base::failbit) != 0)
        {
            try
            {
                // This will set the failbit and throw the exception ios_base::failure.
                os.setstate(err);
            }
            catch (ios_base::failure const&)
            {
                // Do nothing since we want the original exception to be rethrown.
            }
            throw;
        }
        // else: no exception must get out!
    }

    // Needed in the case that no exception has been thrown but the stream state has changed.
    if (err)
        os.setstate(err);
    return os;
}


#endif // HEADER_GENERIC_INSERTER_HPP_INCLUDED
