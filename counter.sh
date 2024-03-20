#!/bin/bash
#Counter lines of code in diffrent files
echo -e "Lines of code in diffrent extensions:"
find . -name "*.cpp" -type f -exec wc -l {} \; | awk 'BEGIN{sum=0}{sum+=$1;}END{print ".cpp: " sum;}'
find . -name "*.h"   -type f -exec wc -l {} \; | awk 'BEGIN{sum=0}{sum+=$1;}END{print ".h  : " sum;}'
find . -name "*.sh"  -type f -exec wc -l {} \; | awk 'BEGIN{sum=0}{sum+=$1;}END{print ".sh : " sum;}'
find . -name "*.md"  -type f -exec wc -l {} \; | awk 'BEGIN{sum=0}{sum+=$1;}END{print ".md : " sum;}'