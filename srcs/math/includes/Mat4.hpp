/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mat4.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:47:34 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/04 14:33:11 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Vec3.hpp"

struct Mat4
{
    float	_m[4][4];

    Mat4();
    Mat4(float diagonal);
    Mat4(const Mat4& other);

    Mat4&	operator=(const Mat4& other);
    Mat4&	operator+=(const Mat4& other);
    Mat4&	operator-=(const Mat4& other);
    Mat4&	operator*=(const Mat4& other);
    Mat4&	operator*=(float t);

    Mat4	operator+(const Mat4& other) const;
    Mat4	operator-(const Mat4& other) const;
    Mat4	operator*(const Mat4& other) const;
    
    Vec3	operator*(const Vec3& v) const;

    Mat4	operator*(float t) const;

    Mat4	transpose() const;
    Mat4	inverse() const;
    float	determinant() const;

    const float*	operator[](int i) const;
    float*			operator[](int i);

    static Mat4	translate(const Vec3& v);
    static Mat4	scale(const Vec3& v);
    static Mat4	rotateX(float angle_radians);
    static Mat4	rotateY(float angle_radians);
    static Mat4	rotateZ(float angle_radians);
    static Mat4	rotate(float angle_radians, const Vec3& axis);
    
    static Mat4	lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
    static Mat4	perspective(float fov, float aspect, float near, float far);
};

Mat4			operator*(float t, const Mat4& m);
std::ostream&	operator<<(std::ostream& out, const Mat4& m);