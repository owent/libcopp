#!/bin/sh

# 检测当前shell编码
CURRENT_ENCODING="GB18030";

ENV_VAR_LIST="LANG LC_CTYPE LC_ALL RC_LANG RC_LC_CTYPE";
UTF8_VAR_LIST="UTF-8 UTF8";

for ENV_VAR in $ENV_VAR_LIST; do
	for UTF8_VAR in $UTF8_VAR_LIST; do
		CURRENT_ENCODING_CHECK="$(set | grep $ENV_VAR=.*$UTF8_VAR -i)";
		if [ ! -z "$CURRENT_ENCODING_CHECK" ]; then
			CURRENT_ENCODING="UTF-8";
			break 2;
		fi
	done
done


$* 2>&1 | iconv -f GB18030 -t $CURRENT_ENCODING;
