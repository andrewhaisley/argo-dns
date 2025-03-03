#!/usr/bin/python3

import sqlite3
import uuid
import sys
import os

def get_horizon_id(conn):
    cur = conn.cursor()
    res = cur.execute('SELECT horizon_id FROM horizon')
    x = res.fetchone()
    return x[0]

def get_zone_ids(conn):
    cur = conn.cursor()
    res = cur.execute('SELECT zone_id FROM zone')
    ids = []
    while True:
        x = res.fetchone()
        if x is None:
            return ids
        else:
            ids.append(x[0])

def get_rr_ids(conn, table, zone_id):
    cur = conn.cursor()
    res = cur.execute('SELECT ' + table + '_id FROM ' + table + ' WHERE zone_id = %s' % zone_id)
    ids = []
    while True:
        x = res.fetchone()
        if x is None:
            return ids
        else:
            ids.append(x[0])

def update_rr_table(conn, table, old_zone_id, new_zone_id):
    cur = conn.cursor()
    rr_ids = get_rr_ids(conn, table, old_zone_id)

    for old_rr_id in rr_ids:
        new_rr_id = uuid.uuid4()
        id_column = table + '_id'
        sql = """
            UPDATE
                %s
            SET
                zone_id = '%s',
                %s = '%s'
            WHERE
                zone_id = '%s'
            AND
                %s = '%s'
        """ % (table, new_zone_id, id_column, new_rr_id, old_zone_id, id_column, old_rr_id)

        res = cur.execute(sql)

def update_zone(conn, horizon_id, old_zone_id):

    new_zone_id = uuid.uuid4()

    rr_tables = ['rr_a', 'rr_aaaa', 'rr_cname', 'rr_dname', 'rr_hinfo', 'rr_mx', 'rr_naptr', 'rr_ns', 'rr_ptr', 'rr_soa', 'rr_srv', 'rr_txt']

    for t in rr_tables:
        update_rr_table(conn, t, old_zone_id, new_zone_id)

    cur = conn.cursor()

    sql = """
        UPDATE
            zone
        SET
            horizon_id = '%s',
            zone_id = '%s'
        WHERE
            zone_id = '%s'
    """ % (horizon_id, new_zone_id, old_zone_id)

    res = cur.execute(sql)


os.system('cat test_zones.sql | sqlite3 ' + sys.argv[1])
conn = sqlite3.connect(sys.argv[1])
horizon_id = get_horizon_id(conn)

for zone_id in get_zone_ids(conn):
    update_zone(conn, horizon_id, zone_id)

conn.commit()
