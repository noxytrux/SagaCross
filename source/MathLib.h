/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Mathlib.h -- Copyright (c) 2005-2006 David Henry
// last modification: feb. 19, 2006
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Declarations for 3D maths object and functions to use with OpenGL.
//
// Provide vector, matrix and quaternion operations.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATHLIB_H__
#define __MATHLIB_H__

#include <cmath>

// Forward declarations
template <typename Real> class Vector3;
template <typename Real> class Matrix4x4;
template <typename Real> class Quaternion;

// Type definitions
enum Axis
{
    kXaxis, kYaxis, kZaxis
};

// Declare a global constant for pi and a few multiples.
const float kPi = 3.14159265358979323846f;
const float k2Pi = kPi * 2.0f;
const float kPiOver2 = kPi / 2.0f;
const float k1OverPi = 1.0f / kPi;
const float k1Over2Pi = 1.0f / k2Pi;
const float kPiOver180 = kPi / 180.0f;
const float k180OverPi = 180.0f / kPi;

// "Wrap" an angle in range -pi...pi by adding the correct multiple
// of 2 pi
template <typename Real>
inline Real wrapPi (Real theta);

// "Safe" inverse trig functions
template <typename Real>
inline Real safeAcos (Real x);

// Set the Euler angle triple to its "canonical" value
template <typename Real>
inline void canonizeEulerAngles (Real &roll, Real &pitch, Real &yaw);

// Convert between degrees and radians
template <typename Real>
inline Real degToRad (Real deg)
{
    return deg * kPiOver180;
}

template <typename Real>
inline Real radToDeg (Real rad)
{
    return rad * k180OverPi;
}

// Convert between "field of view" and "zoom".
// The FOV angle is specified in radians.
template <typename Real>
inline Real fovToZoom (Real fov)
{
    return 1.0f / std::tan (fov * 0.5f);
}

template <typename Real>
inline Real zoomToFov (Real zoom)
{
    return 2.0f * std::atan (1.0f / zoom);
}


/////////////////////////////////////////////////////////////////////////////
//
// class Vector3<Real> - A simple 3D vector class.
//
/////////////////////////////////////////////////////////////////////////////

template <typename Real>
class Vector3
{
public:
    // Constructors
    Vector3 () { }
    Vector3 (Real x, Real y, Real z)
        : _x (x), _y (y), _z (z) { }

    Vector3 (Real x, Real y)
        : _x (x), _y (y), _z (0.0) { }

    Vector3 (Real * v)
        : _x (v[0]), _y (v[1]), _z (v[2]) { }

    Vector3 ( const Real & v)
        : _x (v), _y (v), _z (v) { }

    template <typename Real2 >
    Vector3 ( const Vector3<Real2> & v ) : _x( (Real)v._x ), _y( (Real)v._y ), _z( (Real)v._z )
    {}

public:

    static Vector3<Real> random(){
    return Vector3<Real>( rand()/(Real)RAND_MAX , rand()/(Real)RAND_MAX , rand()/(Real)RAND_MAX );
    }

    // Vector comparison
    inline bool operator== (const Vector3<Real> &v) const;
    inline bool operator!= (const Vector3<Real> &v) const;

    // Vector negation
    inline Vector3<Real> operator- () const;

    // Vector operations
    inline Vector3<Real> operator+ (const Vector3<Real> &v) const;
    inline Vector3<Real> operator- (const Vector3<Real> &v) const;
    inline Vector3<Real> operator/ (const Vector3<Real> &v) const;
    inline Vector3<Real> operator* (const Real s) const;
    inline Vector3<Real> operator/ (const Real s) const;

    // Combined assignment operators to conform to
    // C notation convention
    inline Vector3<Real> &operator+= (const Vector3<Real> &v);
    inline Vector3<Real> &operator-= (const Vector3<Real> &v);
    inline Vector3<Real> &operator= (const Real *v)
    {
        _x = v[0];
        _y = v[1];
        _z = v[2];
        return *this;
    }
    inline Vector3<Real> &operator= (const Real & v)
    {
        _x = _y = _z = v;
        return *this;
    }

    inline bool inside (const Vector3<Real> & v) const
    {
        return (_x<v._x) && (_x>-v._x) && (_y<v._y) && (_y>-v._y) && (_z<v._z) && (_z>-v._z);
    }

    inline Vector3<Real> &operator*= (Real s);
    inline Vector3<Real> &operator*= (const Vector3<Real> &v)
    {
        _x *= v._x;
        _y *= v._y;
        _z *= v._z;
        return *this;
    }

    inline Vector3<Real> operator * ( const Vector3<Real> &v ) const
    {
        return Vector3<Real>( _x * v._x , _y * v._y , _z * v._z );
    }

    inline Vector3<Real> &operator/= (Real s);

    // Accessor.  This allows to use the vector object
    // like an array of Real. For example:
    // Vector3<float> v (...);
    // float f = v[1]; // access to _y
    inline operator const Real *()
    {
        return _v;
    }

public:
    // Other vector operations
    inline bool isZero ();
    inline void normalize ();

    inline Real length ()
    {
        return sqrt( _x * _x + _y * _y + _z * _z );
    }



    inline Real length_cubic ()
    {
        return _x * _x + _y * _y + _z * _z;
    }


public:
    // Member variables
    union
    {
        struct
        {
            Real _x, _y, _z;
        };

        Real _v[3];
    };

};

template <class Real>
class Vector4 : public Vector3<Real>
{
public:

    float _w;

    Vector4( float x = 0.0f , float y = 0.0f , float z = 0.0f , float w = 0.0f ) : Vector3<Real>( x , y , z ) , _w( w )
    {
    }

    Vector4( const Vector3<Real> & v ) : Vector3<Real>( v ), _w( 0.0f )
    {

    }

};

// Predefined Vector3 types
typedef Vector3<int> Vector3i;
typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;

typedef Vector4<int> Vector4i;
typedef Vector4<float> Vector4f;
typedef Vector4<double> Vector4d;

// We provide a global constant zero vector
static const Vector3f kZeroVectorf (0.0f, 0.0f, 0.0f);
static const Vector3d kZeroVectord (0.0, 0.0, 0.0);

//
// Nonmember Vector3 functions
//

template <typename Real>
inline Vector3<Real> operator* (Real k, const Vector3<Real> & v);

template <typename Real>
inline Real VectorMag (const Vector3<Real> &v);

template <typename Real>
inline Real DotProduct (const Vector3<Real> &a, const Vector3<Real> &b);

template <typename Real>
inline Vector3<Real> Reflect (const Vector3<Real> &I, const Vector3<Real> &N){
return I - N * ( 2.0 * DotProduct( N , I ) );
}

template <typename Real>
inline Vector3<Real> CrossProduct (const Vector3<Real> &a, const Vector3<Real> &b);

template <typename Real>
inline Vector3<Real> ComputeNormal (const Vector3<Real> &p1,
                                    const Vector3<Real> &p2, const Vector3<Real> &p3);

template <typename Real>
inline Real Distance (const Vector3<Real> &a, const Vector3<Real> &b);

template <typename Real>
inline Real DistanceSquared (const Vector3<Real> &a, const Vector3<Real> &b);


/////////////////////////////////////////////////////////////////////////////
//
// class Matrix4x4<Real> - Implement a 4x4 Matrix class that can represent
// any 3D affine transformation.
//
/////////////////////////////////////////////////////////////////////////////

template <typename Real>
class Matrix4x4
{
public:
    // Constructor - Initialize the last (never used) row of the matrix
    // so that we can do any operation on matrices on the 3x4 portion
    // and forget that line which will (and should) never change.
    Matrix4x4 ()
        : _h14 (0.0f), _h24 (0.0f), _h34 (0.0f), _tw (1.0f) { }

    // Note that we don't define the copy constructor and let the compiler
    // doing it itself because such initialization is not necessary
    // since the source matrix has its last row already initialized...

public:
    // Public interface
    inline void identity ();
    inline void transpose ();
    inline void invert ();
    inline void setTranslation (const Vector3<Real> &v);

    inline void transform (Vector3<Real> &v) const;
    inline void rotate (Vector3<Real> &v) const;
    inline void inverseRotate (Vector3<Real> &v) const;
    inline void inverseTranslate (Vector3<Real> &v) const;

    inline void fromQuaternion (const Quaternion<Real> &q);

    // Matrix <-> Euler conversions; XYZ rotation order; angles in radians
    inline void fromEulerAngles (Real x, Real y, Real z);
    inline void toEulerAngles (Real &x, Real &y, Real &z) const;

    // Return a base vector from the matrix
    inline Vector3<Real> rightVector () const;
    inline Vector3<Real> upVector () const;
    inline Vector3<Real> forwardVector () const;
    inline Vector3<Real> translationVector () const;

    // Accessor.  This allows to use the matrix object
    // like an array of Real. For example:
    // Matrix4x4<float> mat;
    // float f = mat[4]; // access to _m21
    inline operator const Real *()
    {
        return _m;
    }

public:
    // Member variables

    // The values of the matrix.  Basically the upper 3x3 portion
    // contains a linear transformation, and the last column is the
    // translation portion. Here data is transposed, see the Mathlib.inl
    // for more details.
    union
    {
        struct
        {
            Real _m11, _m12, _m13, _h14;
            Real _m21, _m22, _m23, _h24;
            Real _m31, _m32, _m33, _h34;
            Real _tx,  _ty,  _tz,  _tw;
        };

        // Access to raw packed matrix data (usefull for
        // glLoadMatrixf () and glMultMatrixf ())
        Real _m[16];
    };
};


// Predefined Matrix4x4 types
typedef Matrix4x4<float> Matrix4x4f;
typedef Matrix4x4<double> Matrix4x4d;


//
// Nonmember Matrix4x4 functions
//

// Matrix concatenation
template <typename Real>
inline Matrix4x4<Real> operator* (const Matrix4x4<Real> &a, const Matrix4x4<Real> &b);

template <typename Real>
inline Matrix4x4<Real> &operator*= (Matrix4x4<Real> &a, const Matrix4x4<Real> &b);

// Vector transformation
template <typename Real>
inline Vector3<Real> operator* (const Matrix4x4<Real> &m, const Vector3<Real> &p);

// Transpose matrix
template <typename Real>
inline Matrix4x4<Real> Transpose (const Matrix4x4<Real> &m);

// Invert matrix
template <typename Real>
inline Matrix4x4<Real> Invert (const Matrix4x4<Real> &m);

//
// Matrix-builder functions
//

template <typename Real> inline Matrix4x4<Real> RotationMatrix (Axis axis, Real theta);
template <typename Real> inline Matrix4x4<Real> RotationMatrix (const Vector3<Real> &axis, Real theta);
template <typename Real> inline Matrix4x4<Real> TranslationMatrix (Real x, Real y, Real z);
template <typename Real> inline Matrix4x4<Real> TranslationMatrix (const Vector3<Real> &v);
template <typename Real> inline Matrix4x4<Real> ScaleMatrix (const Vector3<Real> &s);
template <typename Real> inline Matrix4x4<Real> ScaleAlongAxisMatrix (const Vector3<Real> &axis, Real k);
template <typename Real> inline Matrix4x4<Real> ShearMatrix (Axis axis, Real s, Real t);
template <typename Real> inline Matrix4x4<Real> ProjectionMatrix (const Vector3<Real> &n);
template <typename Real> inline Matrix4x4<Real> ReflectionMatrix (Axis axis, Real k);
template <typename Real> inline Matrix4x4<Real> AxisReflectionMatrix (const Vector3<Real> &n);

template <typename Real>
inline Matrix4x4<Real> LookAtMatrix (const Vector3<Real> &camPos,
                                     const Vector3<Real> &target, const Vector3<Real> &camUp);
template <typename Real>
inline Matrix4x4<Real> FrustumMatrix (Real l, Real r, Real b, Real t, Real n, Real f);
template <typename Real>
inline Matrix4x4<Real> PerspectiveMatrix (Real fovY, Real aspect, Real n, Real f);
template <typename Real>
inline Matrix4x4<Real> OrthoMatrix (Real l, Real r, Real b, Real t, Real n, Real f);
template <typename Real>
inline Matrix4x4<Real> OrthoNormalMatrix (const Vector3<Real> &xdir,
        const Vector3<Real> &ydir, const Vector3<Real> &zdir);


/////////////////////////////////////////////////////////////////////////////
//
// class Quaternion<Real> - Implement a quaternion, for purposes of
// representing an angular displacement (orientation) in 3D.
//
/////////////////////////////////////////////////////////////////////////////

template <typename Real>
class Quaternion
{
public:
    // Constructors
    Quaternion ()
    {
        identity ();
    }
    Quaternion (Real w, Real x, Real y, Real z)
        : _w (w), _x (x), _y (y), _z (z) { }

public:
    // Public interface
    inline void identity ();
    inline void normalize ();
    inline void computeW ();
    inline void rotate (Vector3<Real> &v) const;

    bool operator!=( const Quaternion<Real> & p )
    {
        return !( float_equal( _w , p._w ) && float_equal( _x , p._x ) && float_equal( _y , p._y ) && float_equal( _z , p._z ) );
    }

    inline void fromMatrix (const Matrix4x4<Real> &m);

    // Quaternion <-> Euler conversions; XYZ rotation order; angles in radians
    inline void fromEulerAngles (Real x, Real y, Real z);
    inline void toEulerAngles (Real &x, Real &y, Real &z) const;

    inline Real rotationAngle () const;
    inline Vector3<Real> rotationAxis () const;

    // Quaternion operations
    inline Quaternion<Real> operator+ (const Quaternion<Real> &q) const;
    inline Quaternion<Real> &operator+= (const Quaternion<Real> &q);

    inline Quaternion<Real> operator- (const Quaternion<Real> &q) const;
    inline Quaternion<Real> &operator-= (const Quaternion<Real> &q);

    inline Quaternion<Real> operator* (const Quaternion<Real> &q) const;
    inline Quaternion<Real> &operator*= (const Quaternion<Real> &q);

    inline Quaternion<Real> operator* (Real k) const;
    inline Quaternion<Real> &operator*= (Real k);

    inline Quaternion<Real> operator* (const Vector3<Real> &v) const;
    inline Quaternion<Real> &operator*= (const Vector3<Real> &v);

    inline Quaternion<Real> operator/ (Real k) const;
    inline Quaternion<Real> &operator/= (Real k);

    inline Quaternion<Real> operator~ () const; // Quaternion conjugate
    inline Quaternion<Real> operator- () const; // Quaternion negation

    Vector3<Real> GetForwardVector() const
    {
        return Vector3<Real>( 2 * (_x * _z + _w * _y),
                              2 * (_y * _x - _w * _x),
                              1 - 2 * (_x * _x + _y * _y));
    }

    Vector3<Real> GetUpVector() const
    {
        return Vector3<Real>( 2 * (_x * _y - _w * _z),
                              1 - 2 * (_x * _x + _z * _z),
                              2 * (_y * _z + _w * _x));
    }

    Vector3<Real> GetRightVector() const
    {
        return Vector3<Real>( 1 - 2 * (_y * _y + _z * _z),
                              2 * (_x * _y + _w * _z),
                              2 * (_x * _z - _w * _y));
    }

public:
    // Member variables

    // The 4 values of the quaternion.  Normally, it will not
    // be necessary to manipulate these directly.  However,
    // we leave them public, since prohibiting direct access
    // makes some operations, such as file I/O, unnecessarily
    // complicated.

    union
    {
        struct
        {
            Real _w, _x, _y, _z;
        };

        Real _q[4];
    };

};


// Predefined Quaternion types
typedef Quaternion<float> Quaternionf;
typedef Quaternion<double> Quaterniond;

// A global "identity" quaternion constant
static const Quaternionf kQuaternionIdentityf (1.0f, 0.0f, 0.0f, 0.0f);
static const Quaterniond kQuaternionIdentityd (1.0f, 0.0f, 0.0f, 0.0f);

//
// Nonmember Matrix4x functions
//

template <typename Real>
inline Quaternion<Real> operator* (Real k, const Quaternion<Real> &q);

template <typename Real>
inline Real DotProduct (const Quaternion<Real> &a, const Quaternion<Real> &b);

template <typename Real>
inline Quaternion<Real> Conjugate (const Quaternion<Real> &q);

template <typename Real>
inline Quaternion<Real> Inverse (const Quaternion<Real> &q);

template <typename Real>
inline void Negate ( Quaternion<Real> &q);

template <typename Real>
inline Quaternion<Real> RotationQuaternion (Axis axis, Real theta);

template <typename Real>
inline Quaternion<Real> RotationQuaternion (const Vector3<Real> &axis, Real theta);

template <typename Real>
inline Quaternion<Real> Log (const Quaternion<Real> &q);
template <typename Real>
inline Quaternion<Real> Exp (const Quaternion<Real> &q);
template <typename Real>
inline Quaternion<Real> Pow (const Quaternion<Real> &q, Real exponent);

template <typename Real>
inline Quaternion<Real> Slerp (const Quaternion<Real> &q0, const Quaternion<Real> &q1, Real t);
template <typename Real>
inline Quaternion<Real> Squad (const Quaternion<Real> &q0, const Quaternion<Real> &qa,
                               const Quaternion<Real> &qb, const Quaternion<Real> &q1, Real t);
template <typename Real>
inline void Intermediate (const Quaternion<Real> &qprev, const Quaternion<Real> &qcurr,
                          const Quaternion<Real> &qnext, Quaternion<Real> &qa,
                          Quaternion<Real> &qb);

template <typename Real>
inline Quaternion<Real> GetControlQuaternion( const Quaternion<Real>& past, const Quaternion<Real>& current, const Quaternion<Real>& future )
{
    Quaternion<Real> q ( -current._x, -current._y, -current._z, current._w );
    return current * Exp( (Log(q * past) + Log(q * future) ) * -0.25f);
}

template <class T>
Quaternion<T> GenerateRotationFromDirectionVector( Vector3<T> vDirection , bool add_rot = true )
{
    // Step 1. Setup basis vectors describing the rotation given the input vector and assuming an initial up direction of (0, 1, 0)
    Vector3<T> vUp(0.0, 1.0, 0.0);           // Y Up vector
    Vector3<T> vRight = CrossProduct(vUp, vDirection);    // The perpendicular vector to Up and Direction
    vUp = CrossProduct(vDirection, vRight);            // The actual up vector given the direction and the right vector

    // Step 2. Put the three vectors into the matrix to bulid a basis rotation matrix
    // This step isnt necessary, but im adding it because often you would want to convert from matricies to quaternions instead of vectors to quaternions
    // If you want to skip this step, you can use the vector values directly in the quaternion setup below

    T t[16] = { vRight._x, vRight._y, vRight._z, 0.0f,
                vUp._x, vUp._y, vUp._z, 0.0f,
                vDirection._x, vDirection._y, vDirection._z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
              };

    // Step 3. Build a quaternion from the matrix
    Quaternion<T> qrot;

    qrot . fromMatrix( *(Matrix4x4<T>*)t );
    qrot . normalize();

    if( add_rot ){
        qrot *= RotationQuaternion( kXaxis , (float)M_PI_2 );
        qrot . normalize();
    }

    return qrot;
}


// Include inline function definitions
#include "Mathlib.inl"

#endif // __MATHLIB_H__
