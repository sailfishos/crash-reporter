#!/bin/sh

PACKAGE="crash-reporter-ui-tests"

echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>
 <testdefinition version=\"0.1\">
  <suite name=\"$PACKAGE\" domain=\"AutomatedTesting\">
   <set name=\"acceptance_tests\" description=\"Acceptance Tests\">
"

# Append tests.

for FEATUREPATH in `ls */*/*.feature`; do
	if [ -e $FEATUREPATH ]; then
        FEATUREFILE=${FEATUREPATH##*/}
        FEATURENAME=${FEATUREFILE%%.*}
        echo "
    <case type=\"Functional\" name=\"$FEATURENAME\" description=\"$PACKAGE:$FEATUREFILE\" timeout=\"3600\">
     <step>rm -f /home/user/MyDocs/core-dumps/*</step>
     <step>cucumber /usr/share/$PACKAGE/$FEATUREPATH</step>
    </case>
    "
    fi
done

echo "
    <environments>
	 <scratchbox>false</scratchbox>
	 <hardware>true</hardware>
    </environments>

   <get>
    <file>/var/log/syslog</file>
   </get>

  </set>
 </suite>
</testdefinition>
"
