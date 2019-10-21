import struct
import sqlite3
from typing import List


conn = sqlite3.connect('./wikidata/xindex-nocase.db')


def offset_to_title(offset: int) -> str:
    res = conn.execute('select * from pages where offset = ?;', (offset,)).fetchone()
    if not res:
        print('No result found')
        return
    return res[0]


def title_to_offset(title: str) -> int:
    res = conn.execute('select * from pages where title = ?;', (title,)).fetchone()
    if not res:
        print('No result found')
        return
    return res[1]


def get_graph() -> List[int]:
    """
    The first 4 ints are the file header. First the version, next the total number of pages P, then 2 unused.
    After this is P page data sections, each page is placed one after another until the end of the file.

    Pages

    Each page starts with a 4 int page header:

    * The first int is zero and is reserved for the user. I have used this for marking pages as seen and
        referencing the parent page during breadth-first-search path finding. This way no external data
        tables are necessary. Useful when you read the file into a mutable array in memory.
    * The number of links N that the page has.
    * The number of bidirectional links B the page has. These are links where the page being linked to also
        links back to this page. This generally implies a stronger connection between the topics of the two pages.
    * A metadata integer M with a bunch of bit fields and some zeroes that should be ignored for adding future metadata
    """
    with open('./wikidata/indexbi.bin', 'rb') as f:
        data = f.read()
    arr = [x[0] for x in list(struct.iter_unpack('i', data))]
    return arr


def clear_parent_refs(arr):
    """Resets unused int slot per article (used for indicating parent in search) to 0"""
    i = 4
    while i < len(arr):
        arr[i] = 0
        i += 4 + arr[i + 1]


def trace_path(arr: List[int], offset: int) -> List[int]:
    """Trace back a path from destination to start node and return that path of articles"""
    path = []
    path.append(offset)

    # start node denoted with parent = -1
    while arr[offset // 4] != -1:
        offset = arr[offset // 4]
        path.append(offset)

    return path[::-1]
