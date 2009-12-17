# $Id: coll.py 420 2009-04-26 08:04:37Z joe.wreschnig $

"""Generic collision algorithms.

When a box or line is used, the lower bounds are inclusive but the upper
ones are exclusive.
"""

def dot((x1, y1), (x2, y2)):
    return x1 * x2 + y1 * y2

def pointbox((ax, ay), (blx, bly, bhx, bhy)):
    """Check if a point is in an axis-aligned box."""
    return (blx <= ax < bhx) and (bly <= ay < bhy)

def pointcircle((ax, ay), (bx, by, br)):
    """Check if a point is within a circle."""
    x = ax - bx
    y = ay - by
    return (x * x + y * y) <= (br * br)

def circlecircle((ax, ay, ar), (bx, by, br)):
    """Check if a circle intersects another circle."""
    x = ax - bx
    y = ay - by
    return (x * x + y * y) <= (ar * ar + br * br)

def boxbox((alx, aly, ahx, ahy), (blx, bly, bhx, bhy)):
    """Check if a box is within a box."""
    if ahx < blx: return False
    elif alx >= bhx: return False
    elif ahy < bly: return False
    elif aly >= bhy: return False
    else: return True

def circleline((cx, cy, cr), (ax, ay), (bx, by)):
    """Check if a circle and a line segment intersect."""
    if (ax == bx and ay == by):
        return pointcircle((ax, ay), (cx, cy, cr))
    dir = (bx - ax, by - ay)
    diff = (cx - ax, cy - ay)
    t = dot(diff, dir) / dot(dir, dir)
    if t < 0:
        t = 0
    elif t > 1:
        t = 1
    closest = (ax + dir[0] * t, ay + dir[1] * t)
    d = (cx - closest[0], cy - closest[1])
    distsq = dot(d, d)
    return distsq <= cr * cr

def pointpoly((px, py), vertices):
    inside = False
    if len(vertices) < 3:
        return False
    old_x, old_y = vertices[-1]
    for (new_x, new_y) in vertices:
        if (py > min(new_y, old_y)
            and py < max(new_y, old_y)
            and px < max(old_x, new_x)
            and old_y != new_y):
            intersect = ((px - old_x) * (new_x - new_y)
                         / (new_y - old_y)) + old_x
            if old_x == new_x or px < intersect:
                inside = not inside
        old_x, old_y = new_x, new_y
    return inside

def circlepoly(c, vertices):
    v1 = vertices[-1]
    for v2 in vertices:
        if circleline(c, v1, v2):
            return True
        v1 = v2
    return pointpoly((c[0], c[1]), vertices)
        
