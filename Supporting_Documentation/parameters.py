#!/usr/bin/env python

import math

########################################
# Helper functions

stirling_cache = {}

def stirling2(a, b):
    k = (a, b)
    if k in stirling_cache:
        return stirling_cache[k]

    v = None
    if a == b:
        v = 1
    elif b == 0:
        v = 0
    else:
        x = stirling2(a-1, b)
        y = stirling2(a-1, b-1)
        v = b * x + y

    stirling_cache[k] = v
    return v

# evaluate k-th Touchard polynomial at x
def touchard(k, x):
    tmp = 0
    xx = 1
    for i in range(k+1):
        tmp += stirling2(k, i) * xx
        xx *= x
    return tmp


def naive_hors(k, t, r):
    return k * (math.log(t, 2) - math.log(k, 2) - math.log(r, 2))

def strong_hors(k, t, r):
    tmp = 0
    for j in range(k):
        tmp += math.log(t - j, 2) - math.log(k*r - j, 2)
    return tmp

########################################
# Security

def naive_search(k, t, h, rho, is_chosen_leaf=False, is_strong=False):
    if is_strong:
        p = 0
        poisson = math.exp(-rho)
        # TODO: calibrate r_max
        for r in range(1, 200):
            poisson *= rho
            poisson /= r
            p += poisson * math.pow(2, -strong_hors(k, t, r))
        classic = -math.log(p, 2)
    else:
        classic = k * (math.log(t, 2) - math.log(k, 2)) - math.log(touchard(k, rho), 2)

    quantum = classic / 2
    # Hardware costs
    # Always for naive search!
    quantum += h
    if rho < 1:
        quantum += math.log(rho, 2)
    #if not is_chosen_leaf:
    #    quantum += h

    label = "PORS" if is_strong else "HORS"
    label += "" if is_chosen_leaf else " + rand leaf"
    print "[naive search]", label, "classical =", classic
    print "[naive search]", label, "quantum =", quantum

def weak_horst_search(k, t, h, rho, is_chosen_leaf=False, is_strong=False):
    hh = 2**h
    poisson = math.exp(-rho)
    lpoisson = -rho * math.log(math.exp(1), 2)
    best_classic = [None, None]
    best_quantum = [None, None]
    # TODO: calibrate r_max
    for r in range(1, 200):
        poisson *= rho
        poisson /= r
        lpoisson += math.log(rho, 2) - math.log(r, 2)
        base = strong_hors(k, t, r) if is_strong else naive_hors(k, t, r)
        #print "Naive HORS for r =", r, naive_hors(k, t, r)
        #print "Strong HORS for r =", r, strong_hors(k, t, r)
        x = -math.expm1(-hh*poisson)
        #print x
        if x < 1e-30:
            #pexists = math.log(hh*poisson, 2)
            pexists = h + lpoisson
        else:
            pexists = math.log(x, 2)

        classic = base - pexists
        quantum = base / 2.0 - pexists

        if not is_chosen_leaf:
            classic += h
            quantum += h/2.0

        if best_classic[0] is None or classic < best_classic[0]:
            best_classic[0] = classic
            best_classic[1] = r
        if best_quantum[0] is None or quantum < best_quantum[0]:
            best_quantum[0] = quantum
            best_quantum[1] = r

    label = "PORS" if is_strong else "HORS"
    label += "" if is_chosen_leaf else " + rand leaf"
    print "[weak horst search]", label, "classical for r =", best_classic[1], ":", best_classic[0]
    print "[weak horst search]", label, "quantum for r =", best_quantum[1], ":", best_quantum[0]

def weak_msg_search(k, t, h, rho, is_chosen_leaf=False):
    if not is_chosen_leaf:
        # Weak msg search is useless if leaf index is determined by msg!
        return

    # TODO: is_strong ?
    hh = 2**h
    p = float(hh)
    pkappa = -k * math.log(t, 2)
    logq = h + math.log(rho, 2)
    best_classic = [None, None]
    best_quantum = [None, None]
    for kappa in range(1, k+1):
        p *= k
        p /= t
        x = p * touchard(kappa, rho)
        y = -math.expm1(-x)
        base = -math.log(y, 2)

        # Update t! / (t-kappa)!
        pkappa += math.log(t - kappa + 1, 2)
        # Multiply by stirling2(k, kappa)
        ppkappa = pkappa + math.log(stirling2(k, kappa), 2)

        classic = base + max(logq, -ppkappa)
        quantum = base + max(logq, -ppkappa/2.0)

        if best_classic[0] is None or classic < best_classic[0]:
            best_classic[0] = classic
            best_classic[1] = kappa
        if best_quantum[0] is None or quantum < best_quantum[0]:
            best_quantum[0] = quantum
            best_quantum[1] = kappa

    label = "HORS"
    label += "" if is_chosen_leaf else " + rand leaf"
    print "[weak msg search]", label, "classical for kappa =", best_classic[1], ":", best_classic[0]
    print "[weak msg search]", label, "quantum for kappa =", best_quantum[1], ":", best_quantum[0]


def sphincs_security(k, t, hh, d, c, b, rho):
    print "k =", k
    print "t =", t
    print "h =", hh
    print "d =", d
    print "c =", c
    print "b =", b
    print "rho = 2^" + str(math.log(rho, 2))

    print "\n*** derived values ***"
    h = hh*d + c
    print "H =", h
    n = 256
    w = 16
    mu = n / math.log(w, 2)
    l = int(mu + math.floor(math.log(mu*(w-1), 2) / math.log(w, 2)) + 1)
    print "l =", l
    hashcount = 1 + k*(1 + int(math.log(t, 2) - math.floor(math.log(k, 2)))) + d*(l+hh) + c
    size = hashcount*n / 8
    print "maxsize =", size, "bytes"
    if b > 0:
        size += b*n / 8 + 2
        print "max batched size =", size, "bytes"

    print "\n*** security ***"
    for is_chosen_leaf in [True, False]:
        naive_search(k, t, h, rho, is_chosen_leaf)
        weak_horst_search(k, t, h, rho, is_chosen_leaf)
        weak_msg_search(k, t, h, rho, is_chosen_leaf)
        if not is_chosen_leaf:
            print "\n*** Gravity security ***"
        naive_search(k, t, h, rho, is_chosen_leaf, True)
        weak_horst_search(k, t, h, rho, is_chosen_leaf, True)


if __name__ == "__main__":
    print "\n" + "#" * 40

    print "original SPHINCS"
    sphincs_security(32, 2**16,  5, 12,  0,  0, 2**(-10))
    print "\n" + "#" * 40

    print "Gravity-SPHINCS S"
    sphincs_security(24, 2**16,  5,  1, 10,  0, 2**(-5))
    print "\n" + "#" * 40

    print "Gravity-SPHINCS M"
    sphincs_security(32, 2**16,  5,  7, 15,  0, 2**(0))
    print "\n" + "#" * 40

    print "Gravity-SPHINCS L"
    sphincs_security(28, 2**16,  5, 10, 14,  0, 2**(0))


