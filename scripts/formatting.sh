#!/bin/bash

removeEmptyLinesAndTabs() {
  sed 's/[[:space:]]*//g; /^[[:space:]]*$/d'
}

inlineInformation() {
  awk -F "," '''
  BEGIN               {type=""}
  m=/^([[:alpha:]]*):$/ {type=substr($0, 0, length($0)-1)}
  !m                  {print "type:" type "," $0}
  '''
}

csvFormat() {
  sed '''
    s/type://g
    s/Normal://g
    s/Normal2://g
    s/Copy://g
    s/Move://g
    s/CopyAssign://g
    s/MoveAssign://g
    s/Destructor://g
    1i type,Normal,Normal2,Copy,Move,CopyAssign,MoveAssign,Destructor
  '''
}

removeEmptyLinesAndTabs | inlineInformation | uniq | csvFormat
