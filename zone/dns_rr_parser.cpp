// 
// Copyright 2025 Andrew Haisley
// 
// This program is free software: you can redistribute it and/or modify it under the terms 
// of the GNU General Public License as published by the Free Software Foundation, either 
// version 3 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with this program. 
// If not, see https://www.gnu.org/licenses/.
// 
/**
 * automatically generated by rrgen.py at 05:05PM on January 30, 2024 do not edit by hand.
 */

#include <string.h>

#include <boost/lexical_cast.hpp>

#include "types.hpp"
#include "util.hpp"
#include "dns_rr_parser.hpp"
#include "dns_rr_SOA.hpp"
#include "row_rr_soa.hpp"
#include "dns_rr_A.hpp"
#include "row_rr_a.hpp"
#include "dns_rr_AAAA.hpp"
#include "row_rr_aaaa.hpp"
#include "dns_rr_AFSDB.hpp"
#include "row_rr_afsdb.hpp"
#include "dns_rr_NS.hpp"
#include "row_rr_ns.hpp"
#include "dns_rr_MX.hpp"
#include "row_rr_mx.hpp"
#include "dns_rr_CNAME.hpp"
#include "row_rr_cname.hpp"
#include "dns_rr_DNAME.hpp"
#include "row_rr_dname.hpp"
#include "dns_rr_PTR.hpp"
#include "row_rr_ptr.hpp"
#include "dns_rr_TXT.hpp"
#include "row_rr_txt.hpp"
#include "dns_rr_NAPTR.hpp"
#include "row_rr_naptr.hpp"
#include "dns_rr_KX.hpp"
#include "row_rr_kx.hpp"
#include "dns_rr_OPT.hpp"
#include "dns_rr_SRV.hpp"
#include "row_rr_srv.hpp"
#include "dns_rr_HINFO.hpp"
#include "row_rr_hinfo.hpp"
#include "dns_rr_SVCB.hpp"
#include "row_rr_svcb.hpp"
#include "dns_rr_IPSECKEY.hpp"
#include "row_rr_ipseckey.hpp"
#include "dns_rr_LOC.hpp"
#include "row_rr_loc.hpp"
#include "dns_rr_HTTPS.hpp"
#include "row_rr_https.hpp"
#include "dns_rr_CAA.hpp"
#include "row_rr_caa.hpp"
#include "dns_rr_ZONEMD.hpp"
#include "row_rr_zonemd.hpp"
#include "dns_rr_URI.hpp"
#include "row_rr_uri.hpp"
#include "dns_rr_SSHFP.hpp"
#include "row_rr_sshfp.hpp"
#include "dns_rr_TLSA.hpp"
#include "row_rr_tlsa.hpp"
#include "dns_rr_OPENPGPKEY.hpp"
#include "row_rr_openpgpkey.hpp"
#include "dns_rr_DHCID.hpp"
#include "row_rr_dhcid.hpp"
#include "dns_rr_CERT.hpp"
#include "row_rr_cert.hpp"
#include "dns_rr_EUI48.hpp"
#include "row_rr_eui48.hpp"
#include "dns_rr_EUI64.hpp"
#include "row_rr_eui64.hpp"
#include "dns_rr_CSYNC.hpp"
#include "row_rr_csync.hpp"
#include "dns_rr_TKEY.hpp"
#include "dns_rr_TSIG.hpp"
#include "dns_rr_GENERIC.hpp"
#include "row_rr_generic.hpp"


using namespace std;
using namespace adns;

dns_rr_parser::dns_rr_parser()
{
}

dns_rr_parser::~dns_rr_parser()
{
}

shared_ptr<dns_rr> dns_rr_parser::parse(const octet *raw, size_t size, size_t &offset)
{
    unsigned short i, data_length;
    size_t offset_after_rr;

    shared_ptr<dns_name> name = parse_name(raw, size, offset);

    // type
    i = parse_unsigned_short(raw, size, offset);

    // get an instance of the right class based on the rr type
    shared_ptr<dns_rr> res = instantiate(static_cast<dns_rr::type_t>(i));

    res->m_name = name;

    // class - we only support IN but need the value to handle EDNS0 which overloads it 
    // to represent UDP payload side
    res->m_class = static_cast<dns_rr::class_t>(parse_unsigned_short(raw, size, offset));

    // ttl
    res->m_ttl = parse_uint(raw, size, offset);

    // length of the RR specific data
    data_length = parse_unsigned_short(raw, size, offset);

    offset_after_rr = offset + data_length;

    // parse the rr specific data
    parse(res, data_length, raw, offset + data_length, offset);

    // if the offset now isn't what the length count says it should be, fail
    if (offset != offset_after_rr)
    {
        THROW(rr_parser_format_exception, "incorrect resource record length");
    }

    return res;
}

void dns_rr_parser::unparse(octet *raw, unordered_map<dns_name, unsigned short> &name_offsets, const dns_rr &r, size_t size, size_t &offset, bool compress)
{
    unparse_name(*(r.m_name), raw, name_offsets, size, offset, compress);
    unparse_unsigned_short(static_cast<unsigned short>(r.m_type), raw, size, offset);
    unparse_unsigned_short(static_cast<unsigned short>(r.m_class), raw, size, offset);
    unparse_uint(static_cast<uint>(r.m_ttl), raw, size, offset);

    size_t offset_before = offset;
    unparse_unsigned_short(0, raw, size, offset);
        
    // unparse the rr specific data
    unparse(r, raw, name_offsets, size, offset, compress);

    // write in the record length
    unparse_unsigned_short(offset - offset_before - 2, raw, size, offset_before);
}

shared_ptr<dns_rr> dns_rr_parser::instantiate(dns_rr::type_t t)
{
    switch (t)
    {
    case dns_rr::T_SOA_e:
        return std::make_shared<dns_rr_SOA>();
    case dns_rr::T_A_e:
        return std::make_shared<dns_rr_A>();
    case dns_rr::T_AAAA_e:
        return std::make_shared<dns_rr_AAAA>();
    case dns_rr::T_AFSDB_e:
        return std::make_shared<dns_rr_AFSDB>();
    case dns_rr::T_NS_e:
        return std::make_shared<dns_rr_NS>();
    case dns_rr::T_MX_e:
        return std::make_shared<dns_rr_MX>();
    case dns_rr::T_CNAME_e:
        return std::make_shared<dns_rr_CNAME>();
    case dns_rr::T_DNAME_e:
        return std::make_shared<dns_rr_DNAME>();
    case dns_rr::T_PTR_e:
        return std::make_shared<dns_rr_PTR>();
    case dns_rr::T_TXT_e:
        return std::make_shared<dns_rr_TXT>();
    case dns_rr::T_NAPTR_e:
        return std::make_shared<dns_rr_NAPTR>();
    case dns_rr::T_KX_e:
        return std::make_shared<dns_rr_KX>();
    case dns_rr::T_OPT_e:
        return std::make_shared<dns_rr_OPT>();
    case dns_rr::T_SRV_e:
        return std::make_shared<dns_rr_SRV>();
    case dns_rr::T_HINFO_e:
        return std::make_shared<dns_rr_HINFO>();
    case dns_rr::T_SVCB_e:
        return std::make_shared<dns_rr_SVCB>();
    case dns_rr::T_IPSECKEY_e:
        return std::make_shared<dns_rr_IPSECKEY>();
    case dns_rr::T_LOC_e:
        return std::make_shared<dns_rr_LOC>();
    case dns_rr::T_HTTPS_e:
        return std::make_shared<dns_rr_HTTPS>();
    case dns_rr::T_CAA_e:
        return std::make_shared<dns_rr_CAA>();
    case dns_rr::T_ZONEMD_e:
        return std::make_shared<dns_rr_ZONEMD>();
    case dns_rr::T_URI_e:
        return std::make_shared<dns_rr_URI>();
    case dns_rr::T_SSHFP_e:
        return std::make_shared<dns_rr_SSHFP>();
    case dns_rr::T_TLSA_e:
        return std::make_shared<dns_rr_TLSA>();
    case dns_rr::T_OPENPGPKEY_e:
        return std::make_shared<dns_rr_OPENPGPKEY>();
    case dns_rr::T_DHCID_e:
        return std::make_shared<dns_rr_DHCID>();
    case dns_rr::T_CERT_e:
        return std::make_shared<dns_rr_CERT>();
    case dns_rr::T_EUI48_e:
        return std::make_shared<dns_rr_EUI48>();
    case dns_rr::T_EUI64_e:
        return std::make_shared<dns_rr_EUI64>();
    case dns_rr::T_CSYNC_e:
        return std::make_shared<dns_rr_CSYNC>();
    case dns_rr::T_TKEY_e:
        return std::make_shared<dns_rr_TKEY>();
    case dns_rr::T_TSIG_e:
        return std::make_shared<dns_rr_TSIG>();
    case dns_rr::T_GENERIC_e:
    default:
        return std::make_shared<dns_rr_GENERIC>();

    }
}

void dns_rr_parser::parse(const shared_ptr<dns_rr> &rr, unsigned short rr_length, const octet *raw, size_t size, size_t &offset)
{
    switch (rr->get_type())
    {
    case dns_rr::T_SOA_e:
        {
            dns_rr_SOA *r = dynamic_cast<dns_rr_SOA *>(rr.get());
            r->set_mname(parse_name(raw, size, offset));
            r->set_rname(parse_name(raw, size, offset));
            r->set_serial(parse_uint(raw, size, offset));
            r->set_refresh(parse_int(raw, size, offset));
            r->set_retry(parse_int(raw, size, offset));
            r->set_expire(parse_int(raw, size, offset));
            r->set_minimum(parse_uint(raw, size, offset));
        }
        break;
    case dns_rr::T_A_e:
        {
            dns_rr_A *r = dynamic_cast<dns_rr_A *>(rr.get());
            r->set_ip4_addr(parse_ip4_address(raw, size, offset));
        }
        break;
    case dns_rr::T_AAAA_e:
        {
            dns_rr_AAAA *r = dynamic_cast<dns_rr_AAAA *>(rr.get());
            r->set_ip6_addr(parse_ip6_address(raw, size, offset));
        }
        break;
    case dns_rr::T_AFSDB_e:
        {
            dns_rr_AFSDB *r = dynamic_cast<dns_rr_AFSDB *>(rr.get());
            r->set_subtype(parse_unsigned_short(raw, size, offset));
            r->set_hostname(parse_name(raw, size, offset));
        }
        break;
    case dns_rr::T_NS_e:
        {
            dns_rr_NS *r = dynamic_cast<dns_rr_NS *>(rr.get());
            r->set_nsdname(parse_name(raw, size, offset));
        }
        break;
    case dns_rr::T_MX_e:
        {
            dns_rr_MX *r = dynamic_cast<dns_rr_MX *>(rr.get());
            r->set_preference(parse_short(raw, size, offset));
            r->set_exchange(parse_name(raw, size, offset));
        }
        break;
    case dns_rr::T_CNAME_e:
        {
            dns_rr_CNAME *r = dynamic_cast<dns_rr_CNAME *>(rr.get());
            r->set_cname(parse_name(raw, size, offset));
        }
        break;
    case dns_rr::T_DNAME_e:
        {
            dns_rr_DNAME *r = dynamic_cast<dns_rr_DNAME *>(rr.get());
            r->set_dname(parse_name(raw, size, offset));
        }
        break;
    case dns_rr::T_PTR_e:
        {
            dns_rr_PTR *r = dynamic_cast<dns_rr_PTR *>(rr.get());
            r->set_dname(parse_name(raw, size, offset));
        }
        break;
    case dns_rr::T_TXT_e:
        {
            dns_rr_TXT *r = dynamic_cast<dns_rr_TXT *>(rr.get());
            r->set_txt(parse_txt(raw, size, offset));
        }
        break;
    case dns_rr::T_NAPTR_e:
        {
            dns_rr_NAPTR *r = dynamic_cast<dns_rr_NAPTR *>(rr.get());
            r->set_naptr_order(parse_unsigned_short(raw, size, offset));
            r->set_preference(parse_unsigned_short(raw, size, offset));
            r->set_flags(parse_string(raw, size, offset));
            r->set_services(parse_string(raw, size, offset));
            r->set_naptr_regexp(parse_string(raw, size, offset));
            r->set_replacement(parse_name(raw, size, offset));
        }
        break;
    case dns_rr::T_KX_e:
        {
            dns_rr_KX *r = dynamic_cast<dns_rr_KX *>(rr.get());
            r->set_preference(parse_unsigned_short(raw, size, offset));
            r->set_exchanger(parse_name(raw, size, offset));
        }
        break;
    case dns_rr::T_OPT_e:
        {
            dns_rr_OPT *r = dynamic_cast<dns_rr_OPT *>(rr.get());
            r->set_data(parse_buffer(raw, rr_length, size, offset));
        }
        break;
    case dns_rr::T_SRV_e:
        {
            dns_rr_SRV *r = dynamic_cast<dns_rr_SRV *>(rr.get());
            r->set_priority(parse_unsigned_short(raw, size, offset));
            r->set_weight(parse_unsigned_short(raw, size, offset));
            r->set_port(parse_unsigned_short(raw, size, offset));
            r->set_target(parse_name(raw, size, offset));
        }
        break;
    case dns_rr::T_HINFO_e:
        {
            dns_rr_HINFO *r = dynamic_cast<dns_rr_HINFO *>(rr.get());
            r->set_cpu(parse_string(raw, size, offset));
            r->set_os(parse_string(raw, size, offset));
        }
        break;
    case dns_rr::T_SVCB_e:
        {
            dns_rr_SVCB *r = dynamic_cast<dns_rr_SVCB *>(rr.get());
            r->set_priority(parse_unsigned_short(raw, size, offset));
            r->set_target(parse_name(raw, size, offset));
            r->set_svcparams(parse_svcparams(raw, size, offset));
        }
        break;
    case dns_rr::T_IPSECKEY_e:
        {
            dns_rr_IPSECKEY *r = dynamic_cast<dns_rr_IPSECKEY *>(rr.get());
            r->set_ipseckey(parse_ipseckey(raw, size, offset));
        }
        break;
    case dns_rr::T_LOC_e:
        {
            dns_rr_LOC *r = dynamic_cast<dns_rr_LOC *>(rr.get());
            r->set_version(parse_octet(raw, size, offset));
            r->set_size(parse_octet(raw, size, offset));
            r->set_horizontal_precision(parse_octet(raw, size, offset));
            r->set_vertical_precision(parse_octet(raw, size, offset));
            r->set_latitude(parse_int(raw, size, offset));
            r->set_longitude(parse_int(raw, size, offset));
            r->set_altitude(parse_int(raw, size, offset));
        }
        break;
    case dns_rr::T_HTTPS_e:
        {
            dns_rr_HTTPS *r = dynamic_cast<dns_rr_HTTPS *>(rr.get());
            r->set_priority(parse_unsigned_short(raw, size, offset));
            r->set_target(parse_name(raw, size, offset));
            r->set_svcparams(parse_svcparams(raw, size, offset));
        }
        break;
    case dns_rr::T_CAA_e:
        {
            dns_rr_CAA *r = dynamic_cast<dns_rr_CAA *>(rr.get());
            r->set_flags(parse_octet(raw, size, offset));
            r->set_tag(parse_string(raw, size, offset));
            r->set_contents(parse_remaining_buffer(raw, size, offset));
        }
        break;
    case dns_rr::T_ZONEMD_e:
        {
            dns_rr_ZONEMD *r = dynamic_cast<dns_rr_ZONEMD *>(rr.get());
            r->set_soa_serial(parse_uint(raw, size, offset));
            r->set_scheme(parse_octet(raw, size, offset));
            r->set_algorithm(parse_octet(raw, size, offset));
            r->set_digest(parse_remaining_buffer(raw, size, offset));
        }
        break;
    case dns_rr::T_URI_e:
        {
            dns_rr_URI *r = dynamic_cast<dns_rr_URI *>(rr.get());
            r->set_priority(parse_unsigned_short(raw, size, offset));
            r->set_weight(parse_unsigned_short(raw, size, offset));
            r->set_target(parse_remaining_string(raw, size, offset));
        }
        break;
    case dns_rr::T_SSHFP_e:
        {
            dns_rr_SSHFP *r = dynamic_cast<dns_rr_SSHFP *>(rr.get());
            r->set_algorithm(parse_octet(raw, size, offset));
            r->set_fp_type(parse_octet(raw, size, offset));
            r->set_fingerprint(parse_remaining_buffer(raw, size, offset));
        }
        break;
    case dns_rr::T_TLSA_e:
        {
            dns_rr_TLSA *r = dynamic_cast<dns_rr_TLSA *>(rr.get());
            r->set_certifcate_usage(parse_octet(raw, size, offset));
            r->set_selector(parse_octet(raw, size, offset));
            r->set_matching_type(parse_octet(raw, size, offset));
            r->set_certificate_association(parse_remaining_buffer(raw, size, offset));
        }
        break;
    case dns_rr::T_OPENPGPKEY_e:
        {
            dns_rr_OPENPGPKEY *r = dynamic_cast<dns_rr_OPENPGPKEY *>(rr.get());
            r->set_pgpkey(parse_remaining_buffer(raw, size, offset));
        }
        break;
    case dns_rr::T_DHCID_e:
        {
            dns_rr_DHCID *r = dynamic_cast<dns_rr_DHCID *>(rr.get());
            r->set_contents(parse_remaining_buffer(raw, size, offset));
        }
        break;
    case dns_rr::T_CERT_e:
        {
            dns_rr_CERT *r = dynamic_cast<dns_rr_CERT *>(rr.get());
            r->set_certificate_type(parse_unsigned_short(raw, size, offset));
            r->set_key_tag(parse_unsigned_short(raw, size, offset));
            r->set_algorithm(parse_octet(raw, size, offset));
            r->set_certificate(parse_remaining_buffer(raw, size, offset));
        }
        break;
    case dns_rr::T_EUI48_e:
        {
            dns_rr_EUI48 *r = dynamic_cast<dns_rr_EUI48 *>(rr.get());
            r->set_eui48(parse_eui48(raw, size, offset));
        }
        break;
    case dns_rr::T_EUI64_e:
        {
            dns_rr_EUI64 *r = dynamic_cast<dns_rr_EUI64 *>(rr.get());
            r->set_eui64(parse_eui64(raw, size, offset));
        }
        break;
    case dns_rr::T_CSYNC_e:
        {
            dns_rr_CSYNC *r = dynamic_cast<dns_rr_CSYNC *>(rr.get());
            r->set_soa_serial(parse_uint(raw, size, offset));
            r->set_flags(parse_unsigned_short(raw, size, offset));
            r->set_type_bitmap(parse_type_bitmap(raw, size, offset));
        }
        break;
    case dns_rr::T_TKEY_e:
        {
            dns_rr_TKEY *r = dynamic_cast<dns_rr_TKEY *>(rr.get());
            r->set_contents(parse_remaining_buffer(raw, size, offset));
        }
        break;
    case dns_rr::T_TSIG_e:
        {
            dns_rr_TSIG *r = dynamic_cast<dns_rr_TSIG *>(rr.get());
            r->set_contents(parse_remaining_buffer(raw, size, offset));
        }
        break;
    case dns_rr::T_GENERIC_e:
    default:
        {
            dns_rr_GENERIC *r = dynamic_cast<dns_rr_GENERIC *>(rr.get());
            r->set_contents(parse_remaining_buffer(raw, size, offset));
        }
        break;

    }
}

void dns_rr_parser::unparse(const dns_rr &rr, octet *raw, unordered_map<dns_name, unsigned short> &name_offsets, size_t size, size_t &offset, bool compress)
{
    switch (rr.get_type())
    {
    case dns_rr::T_SOA_e:
        {
            const dns_rr_SOA *r = dynamic_cast<const dns_rr_SOA *>(&rr);
            unparse_name(*(r->get_mname()), raw, name_offsets, size, offset, compress);
            unparse_name(*(r->get_rname()), raw, name_offsets, size, offset, compress);
            unparse_uint(r->get_serial(), raw, size, offset);
            unparse_int(r->get_refresh(), raw, size, offset);
            unparse_int(r->get_retry(), raw, size, offset);
            unparse_int(r->get_expire(), raw, size, offset);
            unparse_uint(r->get_minimum(), raw, size, offset);
        }
        break;
    case dns_rr::T_A_e:
        {
            const dns_rr_A *r = dynamic_cast<const dns_rr_A *>(&rr);
            unparse_ip4_address(r->get_ip4_addr(), raw, size, offset);
        }
        break;
    case dns_rr::T_AAAA_e:
        {
            const dns_rr_AAAA *r = dynamic_cast<const dns_rr_AAAA *>(&rr);
            unparse_ip6_address(r->get_ip6_addr(), raw, size, offset);
        }
        break;
    case dns_rr::T_AFSDB_e:
        {
            const dns_rr_AFSDB *r = dynamic_cast<const dns_rr_AFSDB *>(&rr);
            unparse_unsigned_short(r->get_subtype(), raw, size, offset);
            unparse_name(*(r->get_hostname()), raw, name_offsets, size, offset, compress);
        }
        break;
    case dns_rr::T_NS_e:
        {
            const dns_rr_NS *r = dynamic_cast<const dns_rr_NS *>(&rr);
            unparse_name(*(r->get_nsdname()), raw, name_offsets, size, offset, compress);
        }
        break;
    case dns_rr::T_MX_e:
        {
            const dns_rr_MX *r = dynamic_cast<const dns_rr_MX *>(&rr);
            unparse_short(r->get_preference(), raw, offset);
            unparse_name(*(r->get_exchange()), raw, name_offsets, size, offset, compress);
        }
        break;
    case dns_rr::T_CNAME_e:
        {
            const dns_rr_CNAME *r = dynamic_cast<const dns_rr_CNAME *>(&rr);
            unparse_name(*(r->get_cname()), raw, name_offsets, size, offset, compress);
        }
        break;
    case dns_rr::T_DNAME_e:
        {
            const dns_rr_DNAME *r = dynamic_cast<const dns_rr_DNAME *>(&rr);
            unparse_name(*(r->get_dname()), raw, name_offsets, size, offset, compress);
        }
        break;
    case dns_rr::T_PTR_e:
        {
            const dns_rr_PTR *r = dynamic_cast<const dns_rr_PTR *>(&rr);
            unparse_name(*(r->get_dname()), raw, name_offsets, size, offset, compress);
        }
        break;
    case dns_rr::T_TXT_e:
        {
            const dns_rr_TXT *r = dynamic_cast<const dns_rr_TXT *>(&rr);
            unparse_txt(r->get_txt(), raw, size, offset);
        }
        break;
    case dns_rr::T_NAPTR_e:
        {
            const dns_rr_NAPTR *r = dynamic_cast<const dns_rr_NAPTR *>(&rr);
            unparse_unsigned_short(r->get_naptr_order(), raw, size, offset);
            unparse_unsigned_short(r->get_preference(), raw, size, offset);
            unparse_string(r->get_flags(), raw, size, offset);
            unparse_string(r->get_services(), raw, size, offset);
            unparse_string(r->get_naptr_regexp(), raw, size, offset);
            unparse_name(*(r->get_replacement()), raw, name_offsets, size, offset, false);
        }
        break;
    case dns_rr::T_KX_e:
        {
            const dns_rr_KX *r = dynamic_cast<const dns_rr_KX *>(&rr);
            unparse_unsigned_short(r->get_preference(), raw, size, offset);
            unparse_name(*(r->get_exchanger()), raw, name_offsets, size, offset, false);
        }
        break;
    case dns_rr::T_OPT_e:
        {
            const dns_rr_OPT *r = dynamic_cast<const dns_rr_OPT *>(&rr);
            unparse_buffer(r->get_data(), raw, size, offset);
        }
        break;
    case dns_rr::T_SRV_e:
        {
            const dns_rr_SRV *r = dynamic_cast<const dns_rr_SRV *>(&rr);
            unparse_unsigned_short(r->get_priority(), raw, size, offset);
            unparse_unsigned_short(r->get_weight(), raw, size, offset);
            unparse_unsigned_short(r->get_port(), raw, size, offset);
            unparse_name(*(r->get_target()), raw, name_offsets, size, offset, compress);
        }
        break;
    case dns_rr::T_HINFO_e:
        {
            const dns_rr_HINFO *r = dynamic_cast<const dns_rr_HINFO *>(&rr);
            unparse_string(r->get_cpu(), raw, size, offset);
            unparse_string(r->get_os(), raw, size, offset);
        }
        break;
    case dns_rr::T_SVCB_e:
        {
            const dns_rr_SVCB *r = dynamic_cast<const dns_rr_SVCB *>(&rr);
            unparse_unsigned_short(r->get_priority(), raw, size, offset);
            unparse_name(*(r->get_target()), raw, name_offsets, size, offset, compress);
            unparse_svcparams(r->get_svcparams(), raw, size, offset);
        }
        break;
    case dns_rr::T_IPSECKEY_e:
        {
            const dns_rr_IPSECKEY *r = dynamic_cast<const dns_rr_IPSECKEY *>(&rr);
            unparse_ipseckey(r->get_ipseckey(), raw, size, offset);
        }
        break;
    case dns_rr::T_LOC_e:
        {
            const dns_rr_LOC *r = dynamic_cast<const dns_rr_LOC *>(&rr);
            unparse_octet(r->get_version(), raw, size, offset);
            unparse_octet(r->get_size(), raw, size, offset);
            unparse_octet(r->get_horizontal_precision(), raw, size, offset);
            unparse_octet(r->get_vertical_precision(), raw, size, offset);
            unparse_int(r->get_latitude(), raw, size, offset);
            unparse_int(r->get_longitude(), raw, size, offset);
            unparse_int(r->get_altitude(), raw, size, offset);
        }
        break;
    case dns_rr::T_HTTPS_e:
        {
            const dns_rr_HTTPS *r = dynamic_cast<const dns_rr_HTTPS *>(&rr);
            unparse_unsigned_short(r->get_priority(), raw, size, offset);
            unparse_name(*(r->get_target()), raw, name_offsets, size, offset, false);
            unparse_svcparams(r->get_svcparams(), raw, size, offset);
        }
        break;
    case dns_rr::T_CAA_e:
        {
            const dns_rr_CAA *r = dynamic_cast<const dns_rr_CAA *>(&rr);
            unparse_octet(r->get_flags(), raw, size, offset);
            unparse_string(r->get_tag(), raw, size, offset);
            unparse_remaining_buffer(r->get_contents(), raw, size, offset);
        }
        break;
    case dns_rr::T_ZONEMD_e:
        {
            const dns_rr_ZONEMD *r = dynamic_cast<const dns_rr_ZONEMD *>(&rr);
            unparse_uint(r->get_soa_serial(), raw, size, offset);
            unparse_octet(r->get_scheme(), raw, size, offset);
            unparse_octet(r->get_algorithm(), raw, size, offset);
            unparse_remaining_buffer(r->get_digest(), raw, size, offset);
        }
        break;
    case dns_rr::T_URI_e:
        {
            const dns_rr_URI *r = dynamic_cast<const dns_rr_URI *>(&rr);
            unparse_unsigned_short(r->get_priority(), raw, size, offset);
            unparse_unsigned_short(r->get_weight(), raw, size, offset);
            unparse_remaining_string(r->get_target(), raw, size, offset);
        }
        break;
    case dns_rr::T_SSHFP_e:
        {
            const dns_rr_SSHFP *r = dynamic_cast<const dns_rr_SSHFP *>(&rr);
            unparse_octet(r->get_algorithm(), raw, size, offset);
            unparse_octet(r->get_fp_type(), raw, size, offset);
            unparse_remaining_buffer(r->get_fingerprint(), raw, size, offset);
        }
        break;
    case dns_rr::T_TLSA_e:
        {
            const dns_rr_TLSA *r = dynamic_cast<const dns_rr_TLSA *>(&rr);
            unparse_octet(r->get_certifcate_usage(), raw, size, offset);
            unparse_octet(r->get_selector(), raw, size, offset);
            unparse_octet(r->get_matching_type(), raw, size, offset);
            unparse_remaining_buffer(r->get_certificate_association(), raw, size, offset);
        }
        break;
    case dns_rr::T_OPENPGPKEY_e:
        {
            const dns_rr_OPENPGPKEY *r = dynamic_cast<const dns_rr_OPENPGPKEY *>(&rr);
            unparse_remaining_buffer(r->get_pgpkey(), raw, size, offset);
        }
        break;
    case dns_rr::T_DHCID_e:
        {
            const dns_rr_DHCID *r = dynamic_cast<const dns_rr_DHCID *>(&rr);
            unparse_remaining_buffer(r->get_contents(), raw, size, offset);
        }
        break;
    case dns_rr::T_CERT_e:
        {
            const dns_rr_CERT *r = dynamic_cast<const dns_rr_CERT *>(&rr);
            unparse_unsigned_short(r->get_certificate_type(), raw, size, offset);
            unparse_unsigned_short(r->get_key_tag(), raw, size, offset);
            unparse_octet(r->get_algorithm(), raw, size, offset);
            unparse_remaining_buffer(r->get_certificate(), raw, size, offset);
        }
        break;
    case dns_rr::T_EUI48_e:
        {
            const dns_rr_EUI48 *r = dynamic_cast<const dns_rr_EUI48 *>(&rr);
            unparse_eui48(r->get_eui48(), raw, size, offset);
        }
        break;
    case dns_rr::T_EUI64_e:
        {
            const dns_rr_EUI64 *r = dynamic_cast<const dns_rr_EUI64 *>(&rr);
            unparse_eui64(r->get_eui64(), raw, size, offset);
        }
        break;
    case dns_rr::T_CSYNC_e:
        {
            const dns_rr_CSYNC *r = dynamic_cast<const dns_rr_CSYNC *>(&rr);
            unparse_uint(r->get_soa_serial(), raw, size, offset);
            unparse_unsigned_short(r->get_flags(), raw, size, offset);
            unparse_type_bitmap(r->get_type_bitmap(), raw, size, offset);
        }
        break;
    case dns_rr::T_TKEY_e:
        {
            const dns_rr_TKEY *r = dynamic_cast<const dns_rr_TKEY *>(&rr);
            unparse_remaining_buffer(r->get_contents(), raw, size, offset);
        }
        break;
    case dns_rr::T_TSIG_e:
        {
            const dns_rr_TSIG *r = dynamic_cast<const dns_rr_TSIG *>(&rr);
            unparse_remaining_buffer(r->get_contents(), raw, size, offset);
        }
        break;
    case dns_rr::T_GENERIC_e:
    default:
        {
            const dns_rr_GENERIC *r = dynamic_cast<const dns_rr_GENERIC *>(&rr);
            unparse_remaining_buffer(r->get_contents(), raw, size, offset);
        }
        break;

    }
}

shared_ptr<dns_rr> dns_rr_parser::from_json(json j)
{
    shared_ptr<dns_rr> res = instantiate(dns_rr::string_to_type(j["type"]));

    res->m_class = dns_rr::C_IN_e;
    res->m_ttl = int(j["ttl"]);
    res->m_name = make_shared<dns_name>(dns_name(j["name"]));

    try
    {
        res->m_id = boost::lexical_cast<uuid>(string(j["rr_id"]));
    }
    catch (json_exception &e)
    {
    }

    switch (res->get_type())
    {
    case dns_rr::T_SOA_e:
        {
            dns_rr_SOA *r = dynamic_cast<dns_rr_SOA *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_A_e:
        {
            dns_rr_A *r = dynamic_cast<dns_rr_A *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_AAAA_e:
        {
            dns_rr_AAAA *r = dynamic_cast<dns_rr_AAAA *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_AFSDB_e:
        {
            dns_rr_AFSDB *r = dynamic_cast<dns_rr_AFSDB *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_NS_e:
        {
            dns_rr_NS *r = dynamic_cast<dns_rr_NS *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_MX_e:
        {
            dns_rr_MX *r = dynamic_cast<dns_rr_MX *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_CNAME_e:
        {
            dns_rr_CNAME *r = dynamic_cast<dns_rr_CNAME *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_DNAME_e:
        {
            dns_rr_DNAME *r = dynamic_cast<dns_rr_DNAME *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_PTR_e:
        {
            dns_rr_PTR *r = dynamic_cast<dns_rr_PTR *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_TXT_e:
        {
            dns_rr_TXT *r = dynamic_cast<dns_rr_TXT *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_NAPTR_e:
        {
            dns_rr_NAPTR *r = dynamic_cast<dns_rr_NAPTR *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_KX_e:
        {
            dns_rr_KX *r = dynamic_cast<dns_rr_KX *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_OPT_e:
        {
            dns_rr_OPT *r = dynamic_cast<dns_rr_OPT *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_SRV_e:
        {
            dns_rr_SRV *r = dynamic_cast<dns_rr_SRV *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_HINFO_e:
        {
            dns_rr_HINFO *r = dynamic_cast<dns_rr_HINFO *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_SVCB_e:
        {
            dns_rr_SVCB *r = dynamic_cast<dns_rr_SVCB *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_IPSECKEY_e:
        {
            dns_rr_IPSECKEY *r = dynamic_cast<dns_rr_IPSECKEY *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_LOC_e:
        {
            dns_rr_LOC *r = dynamic_cast<dns_rr_LOC *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_HTTPS_e:
        {
            dns_rr_HTTPS *r = dynamic_cast<dns_rr_HTTPS *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_CAA_e:
        {
            dns_rr_CAA *r = dynamic_cast<dns_rr_CAA *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_ZONEMD_e:
        {
            dns_rr_ZONEMD *r = dynamic_cast<dns_rr_ZONEMD *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_URI_e:
        {
            dns_rr_URI *r = dynamic_cast<dns_rr_URI *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_SSHFP_e:
        {
            dns_rr_SSHFP *r = dynamic_cast<dns_rr_SSHFP *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_TLSA_e:
        {
            dns_rr_TLSA *r = dynamic_cast<dns_rr_TLSA *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_OPENPGPKEY_e:
        {
            dns_rr_OPENPGPKEY *r = dynamic_cast<dns_rr_OPENPGPKEY *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_DHCID_e:
        {
            dns_rr_DHCID *r = dynamic_cast<dns_rr_DHCID *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_CERT_e:
        {
            dns_rr_CERT *r = dynamic_cast<dns_rr_CERT *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_EUI48_e:
        {
            dns_rr_EUI48 *r = dynamic_cast<dns_rr_EUI48 *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_EUI64_e:
        {
            dns_rr_EUI64 *r = dynamic_cast<dns_rr_EUI64 *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_CSYNC_e:
        {
            dns_rr_CSYNC *r = dynamic_cast<dns_rr_CSYNC *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_TKEY_e:
        {
            dns_rr_TKEY *r = dynamic_cast<dns_rr_TKEY *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_TSIG_e:
        {
            dns_rr_TSIG *r = dynamic_cast<dns_rr_TSIG *>(res.get());
            r->from_json(j);
        }
        break;
    case dns_rr::T_GENERIC_e:
    default:
        {
            dns_rr_GENERIC *r = dynamic_cast<dns_rr_GENERIC *>(res.get());
            r->from_json(j);
        }
        break;

    }

    return res;
}
