#!/bin/bash

ECHO="/bin/echo -e"

if [[ -r /etc/profile.d/CP.sh ]]; then
	source /etc/profile.d/CP.sh
elif [[ -r /opt/CPshared/5.0/tmp/.CPprofile.sh ]]; then
	source /opt/CPshared/5.0/tmp/.CPprofile.sh
elif [[ -r $CPDIR/tmp/.CPprofile.sh ]]; then
	source $CPDIR/tmp/.CPprofile.sh
else
	$ECHO "\\nError: Unable to find \$CPDIR/tmp/.CPprofile.sh"
	$ECHO "Verify this file exists and you can read it\\n"
	exit 1
fi

if [[ $(uname -s) != "Linux" ]]; then
	$ECHO "\\nError: This is not running on Linux\\n"
	exit 1
fi

if [[ ! -f /bin/clish ]]; then
	$ECHO "\\nError: This is not Gaia\\n"
	exit 1
fi

if [[ $($CPDIR/bin/cpprod_util FwIsFirewallMgmt) != *"1"* ]]; then
	$ECHO "\\nThis script only works on Management servers\\n"
	exit 1
fi

VERSION=$($CPDIR/bin/cpprod_util CPPROD_GetValue CPshared VersionText 1)
if (( $($ECHO "${VERSION:1} < 80" | bc -l) )); then
	$ECHO "\\nError: This server is on a version lower than R80.x"
	$ECHO "Follow sk105708 for R77.30 and lower\\n"
	exit 1
fi

if [[ ! -f ascii_check ]]; then
	$ECHO "\\nError: ascii_check file is required and should have been in the tar file\\n"
	exit 1
fi

$MDS_FWDIR/scripts/cpm_status.sh no_print
ret=$?
if [[ $ret == "13" ]]; then
	$ECHO "\\nError: CPM is not running"
	$ECHO "Start CPM and wait a few minutes\\n"
	exit 13
fi

if [[ $ret == "11" || $ret == "12" ]]; then
	$ECHO "\\nError: CPM is not up and ready yet\\n"
	exit 11
fi

$ECHO "Exporting rule names and comments...\\n"

psql_client cpm postgres -t -o check_rules -c "select cma.name,dle.name,acc.name,comments from accessctrlrule_data acc, \
(select owner,entity from rulebaseentitylocalinstance_data where not deleted and dlesession=0) ent, \
(select name,objid from dleobjectderef_data where not deleted and dlesession=0) dle, \
(select name,objid from domainbase_data where dlesession = 0 and not deleted) cma \
where not deleted and dlesession=0 and dle.objid = ent.owner and ent.entity = acc.objid and cma.objid = acc.domainid order by dle.name;"

./ascii_check -r check_rules
if [[ "$?" == "0" ]]; then
	rm -rf check_rules
fi
