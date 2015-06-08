#ifndef _RANDOM_H_
#define _RANDOM_H_
/*!
 * \file low_cache.h
 *
 * \brief Tirage au hasard dans un intervalle
 * 
 * \author Jean-Paul Rigault 
 *
 * $Id: low_cache.h,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include <stdlib.h>
#include <time.h>
#include <assert.h>

/*! RANDOM(m, n) tire un nombre au hasard dans l'intervalle [m,n[. */
static inline unsigned int RANDOM(int m, int n)
{
    return m + rand() % (n - m);
}

#endif /* _RANDOM_H_ */ 
