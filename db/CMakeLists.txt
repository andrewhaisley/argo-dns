cmake_minimum_required(VERSION 3.5)

include_directories(
    .
    ../acore
    ../json)

add_library(
    db
    column.cpp
    connection.cpp
    connection_guard.cpp
    connection_pool.cpp
    foreign_key.cpp
    mongodb_row_helper.cpp
    mysql_row_helper.cpp
    postgres_row_helper.cpp
    row_address_list_address.cpp
    row_address_list.cpp
    row_address_list_network.cpp
    row_basic_config.cpp
    row_cache_config.cpp
    row_control_server.cpp
    row.cpp
    row_dns_client.cpp
    row_dns_server.cpp
    row_horizon.cpp
    row_root_server.cpp
    row_rr_aaaa.cpp
    row_rr_a.cpp
    row_rr_afsdb.cpp
    row_rr_caa.cpp
    row_rr_cert.cpp
    row_rr_cname.cpp
    row_rr_csync.cpp
    row_rr_dhcid.cpp
    row_rr_dname.cpp
    row_rr_eui48.cpp
    row_rr_eui64.cpp
    row_rr_generic.cpp
    row_rr_hinfo.cpp
    row_rr_history.cpp
    row_rr_https.cpp
    row_rr_ipseckey.cpp
    row_rr_kx.cpp
    row_rr_loc.cpp
    row_rr_mx.cpp
    row_rr_naptr.cpp
    row_rr_ns.cpp
    row_rr_openpgpkey.cpp
    row_rr_ptr.cpp
    row_rr_soa.cpp
    row_rr_srv.cpp
    row_rr_sshfp.cpp
    row_rr_svcb.cpp
    row_rr_tlsa.cpp
    row_rr_txt.cpp
    row_rr_uri.cpp
    row_rr_zonemd.cpp
    row_schema_version.cpp
    row_server.cpp
    row_server_socket_address.cpp
    row_zone.cpp
    schema_manager.cpp
    sqlite_row_helper.cpp
    table.cpp
    transaction.cpp)
