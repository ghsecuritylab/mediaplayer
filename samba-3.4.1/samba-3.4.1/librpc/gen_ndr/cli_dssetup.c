/*
 * Unix SMB/CIFS implementation.
 * client auto-generated by pidl. DO NOT MODIFY!
 */

#include "includes.h"
#include "../librpc/gen_ndr/cli_dssetup.h"

NTSTATUS rpccli_dssetup_DsRoleGetPrimaryDomainInformation(struct rpc_pipe_client *cli,
							  TALLOC_CTX *mem_ctx,
							  enum dssetup_DsRoleInfoLevel level /* [in]  */,
							  union dssetup_DsRoleInfo *info /* [out] [unique,switch_is(level)] */,
							  WERROR *werror)
{
	struct dssetup_DsRoleGetPrimaryDomainInformation r;
	NTSTATUS status;

	/* In parameters */
	r.in.level = level;

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleGetPrimaryDomainInformation, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLEGETPRIMARYDOMAININFORMATION,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleGetPrimaryDomainInformation, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */
	if (info && r.out.info) {
		*info = *r.out.info;
	}

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_dssetup_DsRoleDnsNameToFlatName(struct rpc_pipe_client *cli,
						TALLOC_CTX *mem_ctx,
						WERROR *werror)
{
	struct dssetup_DsRoleDnsNameToFlatName r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleDnsNameToFlatName, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLEDNSNAMETOFLATNAME,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleDnsNameToFlatName, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_dssetup_DsRoleDcAsDc(struct rpc_pipe_client *cli,
				     TALLOC_CTX *mem_ctx,
				     WERROR *werror)
{
	struct dssetup_DsRoleDcAsDc r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleDcAsDc, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLEDCASDC,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleDcAsDc, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_dssetup_DsRoleDcAsReplica(struct rpc_pipe_client *cli,
					  TALLOC_CTX *mem_ctx,
					  WERROR *werror)
{
	struct dssetup_DsRoleDcAsReplica r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleDcAsReplica, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLEDCASREPLICA,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleDcAsReplica, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_dssetup_DsRoleDemoteDc(struct rpc_pipe_client *cli,
				       TALLOC_CTX *mem_ctx,
				       WERROR *werror)
{
	struct dssetup_DsRoleDemoteDc r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleDemoteDc, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLEDEMOTEDC,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleDemoteDc, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_dssetup_DsRoleGetDcOperationProgress(struct rpc_pipe_client *cli,
						     TALLOC_CTX *mem_ctx,
						     WERROR *werror)
{
	struct dssetup_DsRoleGetDcOperationProgress r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleGetDcOperationProgress, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLEGETDCOPERATIONPROGRESS,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleGetDcOperationProgress, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_dssetup_DsRoleGetDcOperationResults(struct rpc_pipe_client *cli,
						    TALLOC_CTX *mem_ctx,
						    WERROR *werror)
{
	struct dssetup_DsRoleGetDcOperationResults r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleGetDcOperationResults, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLEGETDCOPERATIONRESULTS,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleGetDcOperationResults, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_dssetup_DsRoleCancel(struct rpc_pipe_client *cli,
				     TALLOC_CTX *mem_ctx,
				     WERROR *werror)
{
	struct dssetup_DsRoleCancel r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleCancel, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLECANCEL,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleCancel, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_dssetup_DsRoleServerSaveStateForUpgrade(struct rpc_pipe_client *cli,
							TALLOC_CTX *mem_ctx,
							WERROR *werror)
{
	struct dssetup_DsRoleServerSaveStateForUpgrade r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleServerSaveStateForUpgrade, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLESERVERSAVESTATEFORUPGRADE,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleServerSaveStateForUpgrade, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_dssetup_DsRoleUpgradeDownlevelServer(struct rpc_pipe_client *cli,
						     TALLOC_CTX *mem_ctx,
						     WERROR *werror)
{
	struct dssetup_DsRoleUpgradeDownlevelServer r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleUpgradeDownlevelServer, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLEUPGRADEDOWNLEVELSERVER,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleUpgradeDownlevelServer, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

NTSTATUS rpccli_dssetup_DsRoleAbortDownlevelServerUpgrade(struct rpc_pipe_client *cli,
							  TALLOC_CTX *mem_ctx,
							  WERROR *werror)
{
	struct dssetup_DsRoleAbortDownlevelServerUpgrade r;
	NTSTATUS status;

	/* In parameters */

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_IN_DEBUG(dssetup_DsRoleAbortDownlevelServerUpgrade, &r);
	}

	status = cli->dispatch(cli,
				mem_ctx,
				&ndr_table_dssetup,
				NDR_DSSETUP_DSROLEABORTDOWNLEVELSERVERUPGRADE,
				&r);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_OUT_DEBUG(dssetup_DsRoleAbortDownlevelServerUpgrade, &r);
	}

	if (NT_STATUS_IS_ERR(status)) {
		return status;
	}

	/* Return variables */

	/* Return result */
	if (werror) {
		*werror = r.out.result;
	}

	return werror_to_ntstatus(r.out.result);
}

