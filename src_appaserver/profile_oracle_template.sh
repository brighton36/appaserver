ORACLE_BASE=/u01/app/oracle
ORACLE_HOME=${ORACLE_BASE}/product/8.0.5
ORACLE_SID=ora1
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${ORACLE_HOME}/lib:${ORACLE_HOME}/precomp/lib
PATH=$PATH:${ORACLE_HOME}/bin

export ORACLE_BASE ORACLE_HOME ORACLE_SID LD_LIBRARY_PATH PATH
