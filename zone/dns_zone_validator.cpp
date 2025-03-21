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
#include "dns_zone_validator.hpp"
#include "dns_horizon.hpp"

using namespace std;
using namespace adns;

void dns_zone_validator::validate_insert(const dns_zone &z)
{
    // name has at least one label
    if (z.get_name()->size() < 1)
    {
        THROW(dns_zone_validator_exception, "name is empty");
    }

    // all new records are for the zone
    for (auto const &rr : z.get_pending_resource_records())
    {
        if (*z.get_name() != *rr->get_name())
        {
            if (!rr->get_name()->is_subdomain_of(*z.get_name()))
            {
                THROW(dns_zone_validator_exception, "resource record has out of zone name", rr->to_string());
            }
        }
    }

    // has one and only one SOA and it's for the zone (not a sub-zone etc)
    int n = 0;

    for (auto const &rr : z.get_pending_resource_records())
    {
        if (rr->get_type() == dns_rr::T_SOA_e)
        {
            if (++n > 1)
            {
                THROW(dns_zone_validator_exception, "more than one SOA record found", rr->to_string());
            }

            if (*z.get_name() != *rr->get_name())
            {
                THROW(dns_zone_validator_exception, "SOA record name doesn't match zone", rr->to_string());
            }
        }
    }

    n = 0;

    // has at least one NS for the zone itself
    for (auto const &rr : z.get_pending_resource_records())
    {
        if (rr->get_type() == dns_rr::T_NS_e)
        {
            if (*z.get_name() == *rr->get_name())
            {
                n++;
                break;
            }
        }
    }

    if (n == 0)
    {
        THROW(dns_zone_validator_exception, "no NS records found for zone");
    }

    // MX records are for the zone only (not a sub-name)
    for (auto const &rr : z.get_pending_resource_records())
    {
        if (rr->get_type() == dns_rr::T_MX_e)
        {
            if (*z.get_name() != *rr->get_name())
            {
                THROW(dns_zone_validator_exception, "MX record name doesn't match zone", rr->to_string());
            }
        }
    }

    set<dns_name> cdname_exists;

    // at most one CNAME or DNAME per name
    for (auto const &rr : z.get_pending_resource_records())
    {
        if (rr->get_type() == dns_rr::T_CNAME_e || rr->get_type() == dns_rr::T_DNAME_e)
        {
            auto name = *(rr->get_name());
            if (cdname_exists.count(name) == 1)
            {
                THROW(dns_zone_validator_exception, "more than one CNAME or DNAME for name found", rr->to_string());
            }
            cdname_exists.insert(name);
        }
    }
}

void dns_zone_validator::validate_update(const dns_zone &z)
{
    // if the zone doesn't have any pending resource records then, since name changes
    // aren't allowed, we can assume it's still valid. If the change is to make it
    // forwarded then there's no reason to insist existing resource records should
    // be deleted.
    if (z.get_pending_resource_records().size() == 0)
    {
        return;
    }
    else
    {
        // since update replaces all existing resource records, the insert
        // checks are the ones needed.
        validate_insert(z);
    }
}

void dns_zone_validator::validate_insert_record(const dns_zone &z, const std::shared_ptr<dns_rr> &rr)
{
    if (*z.get_name() != *rr->get_name())
    {
        if (!rr->get_name()->is_subdomain_of(*z.get_name()))
        {
            THROW(dns_zone_validator_exception, "resource record has out of zone name", rr->to_string());
        }
    }

    if (rr->get_type() == dns_rr::T_SOA_e)
    {
        THROW(dns_zone_validator_exception, "can't insert extra SOA record", rr->to_string());
    }

    if (rr->get_type() == dns_rr::T_MX_e)
    {
        if (*z.get_name() != *rr->get_name())
        {
            THROW(dns_zone_validator_exception, "MX record name doesn't match zone", rr->to_string());
        }
    }

    if (rr->get_type() == dns_rr::T_CNAME_e)
    {
        if (z.get_CNAME(*rr->get_name()))
        {
            THROW(dns_zone_validator_exception, "duplicate CNAME record", rr->to_string());
        }
    }

    if (rr->get_type() == dns_rr::T_DNAME_e)
    {
        if (z.get_DNAME(*rr->get_name()))
        {
            THROW(dns_zone_validator_exception, "duplicate DNAME record", rr->to_string());
        }
    }
}

void dns_zone_validator::validate_update_record(const dns_zone &z, const std::shared_ptr<dns_rr> &rr)
{
    auto old_rr = z.get_resource_record(rr->get_id());

    if (!old_rr)
    {
        THROW(dns_zone_validator_exception, "attempt to update non existent resource record", rr->to_string());
    }

    if (rr->get_type() != old_rr->get_type())
    {
        THROW(dns_zone_validator_exception, "can't update resource record type", rr->to_string());
    }

    if (*rr->get_name() == *old_rr->get_name())
    {
        // if the name hasn't changed, then no need to re-do any checks
        return;
    }

    if (*z.get_name() != *rr->get_name())
    {
        if (!rr->get_name()->is_subdomain_of(*z.get_name()))
        {
            THROW(dns_zone_validator_exception, "resource record has out of zone name", rr->to_string());
        }
    }

    if (rr->get_type() == dns_rr::T_SOA_e)
    {
        THROW(dns_zone_validator_exception, "SOA name doesn't match zone", rr->to_string());
    }

    if (rr->get_type() == dns_rr::T_MX_e)
    {
        THROW(dns_zone_validator_exception, "MX record name doesn't match zone", rr->to_string());
    }

    if (z.get_CNAME(*rr->get_name()))
    {
        THROW(dns_zone_validator_exception, "attempt to update to duplicate CNAME record", rr->to_string());
    }

    if (z.get_DNAME(*rr->get_name()))
    {
        THROW(dns_zone_validator_exception, "attempt to update to duplicate DNAME record", rr->to_string());
    }
}

void dns_zone_validator::validate_delete_record(const dns_zone &z, const std::shared_ptr<dns_rr> &rr)
{
    auto old_rr = z.get_resource_record(rr->get_id());

    if (!old_rr)
    {
        THROW(dns_zone_validator_exception, "attempt to delete non existent resource record", rr->to_string());
    }

    if (old_rr->get_type() == dns_rr::T_SOA_e)
    {
        THROW(dns_zone_validator_exception, "can't delete SOA record", old_rr->to_string());
    }

    if (old_rr->get_type() == dns_rr::T_NS_e)
    {
        if (z.get_NS().size() == 1)
        {
            THROW(dns_zone_validator_exception, "can't delete only NS record", old_rr->to_string());
        }
    }
}
