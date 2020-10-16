(*!------------------------------------------------------------
 * [[APP_NAME]] ([[APP_URL]])
 *
 * @link      [[APP_REPOSITORY_URL]]
 * @copyright Copyright (c) [[COPYRIGHT_YEAR]] [[COPYRIGHT_HOLDER]]
 * @license   [[LICENSE_URL]] ([[LICENSE]])
 *------------------------------------------------------------- *)
unit AuthController;

interface

{$MODE OBJFPC}
{$H+}

uses

    fano;

type

    (*!-----------------------------------------------
     * controller that handle route :
     * /auth
     *
     * See Routes/Auth/routes.inc
     *
     * @author [[AUTHOR_NAME]] <[[AUTHOR_EMAIL]]>
     *------------------------------------------------*)
    TAuthController = class(TAbstractController)
    private
        fAuth : IAuth;
        fTokenGenerator : ITokenGenerator;
        fIssuer : string;

        function handleAuthorizedRequest(
            const response : IResponse;
            const username : string
        ) : IResponse;
    public
        constructor create(
            const auth : IAuth;
            const tokenGenerator : ITokenGenerator;
            const issuer : string
        );

        function handleRequest(
            const request : IRequest;
            const response : IResponse;
            const args : IRouteArgsReader
        ) : IResponse; override;
    end;

implementation

uses

    fpjson;

    constructor TAuthController.create(
        const auth : IAuth;
        const tokenGenerator : ITokenGenerator;
        const issuer : string
    );
    begin
        fAuth := auth;
        fTokenGenerator := tokenGenerator;
        fIssuer := issuer;
    end;

    function TAuthController.handleAuthorizedRequest(
        const response : IResponse;
        const username : string
    ) : IResponse;
    var token : string;
        payload : TJSONObject;
    begin
        payload := TJSONObject.create([
            'iss', fIssuer,
            'sub', username
        ]);
        try
            token := fTokenGenerator.generateToken(payload);
            result := TJsonResponse.create(
                response.headers(),
                '{"status":"OK","token" : "' + token + '"}'
            );
        finally
            payload.free();
        end;

    end;

    function TAuthController.handleRequest(
        const request : IRequest;
        const response : IResponse;
        const args : IRouteArgsReader
    ) : IResponse;
    var cred : TCredential;
    begin
        cred.username := request.getParsedBodyParam('user');
        cred.password := request.getParsedBodyParam('passw');
        if fAuth.auth(cred) then
        begin
            result := handleAuthorizedRequest(response, cred.username);
        end else
        begin
            raise EForbidden.create('You are not authorized.');
        end;
    end;

end.
