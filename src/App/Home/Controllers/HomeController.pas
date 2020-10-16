(*!------------------------------------------------------------
 * [[APP_NAME]] ([[APP_URL]])
 *
 * @link      [[APP_REPOSITORY_URL]]
 * @copyright Copyright (c) [[COPYRIGHT_YEAR]] [[COPYRIGHT_HOLDER]]
 * @license   [[LICENSE_URL]] ([[LICENSE]])
 *------------------------------------------------------------- *)
unit HomeController;

interface

{$MODE OBJFPC}
{$H+}

uses

    fano;

type

    (*!-----------------------------------------------
     * controller that handle route :
     * /home
     *
     * See Routes/Home/routes.inc
     *
     * @author [[AUTHOR_NAME]] <[[AUTHOR_EMAIL]]>
     *------------------------------------------------*)
    THomeController = class(TAbstractController)
    private
        fCredentialKey : ShortString;
    public
        constructor create(const keyname : ShortString);
        function handleRequest(
            const request : IRequest;
            const response : IResponse;
            const args : IRouteArgsReader
        ) : IResponse; override;
    end;

implementation

    constructor THomeController.create(const keyname : ShortString);
    begin
        fCredentialKey := keyname;
    end;

    function THomeController.handleRequest(
        const request : IRequest;
        const response : IResponse;
        const args : IRouteArgsReader
    ) : IResponse;
    var username : string;
    begin
        //if we get here, JWT token successfully verified and
        //and request will contain username from JWT
        username := request.getParam(fCredentialKey);
        response.body().write('Hello ' + username);
        result := response;
    end;

end.
