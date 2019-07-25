#pragma once

namespace sc {

    class SCNonCopyable {

    protected:

        SCNonCopyable() = default;
        ~SCNonCopyable() = default;

    private:

        SCNonCopyable(const SCNonCopyable &) = delete;
        SCNonCopyable &operator =(const SCNonCopyable &) = delete;
        SCNonCopyable(SCNonCopyable &&) = delete;
        SCNonCopyable &operator= (SCNonCopyable &&) = delete;

    };
}