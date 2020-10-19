(*!------------------------------------------------------------
 * [[APP_NAME]] ([[APP_URL]])
 *
 * @link      [[APP_REPOSITORY_URL]]
 * @copyright Copyright (c) [[COPYRIGHT_YEAR]] [[COPYRIGHT_HOLDER]]
 * @license   [[LICENSE_URL]] ([[LICENSE]])
 *------------------------------------------------------------- *)

{%dependencies.inc}

//setup Http Bearer authentication middleware with JWT
container.add(
    'bearerAuth',
    TBearerAuthMiddlewareFactory.create()
        .verifier(container['jwtVerifier'] as ITokenVerifier)
        .credentialKey((container['config'] as IAppConfiguration).getString('credentialKey'))
);
