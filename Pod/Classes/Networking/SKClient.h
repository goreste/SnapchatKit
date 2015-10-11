//
//  SKClient.h
//  SnapchatKit
//
//  Created by Tanner on 5/5/15.
//  Copyright (c) 2015 Tanner Bennett. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreGraphics/CGGeometry.h>
#import "SnapchatKit-Constants.h"

#import "SKSession.h"

extern NSString *SKMakeCapserSignature(NSDictionary *params, NSString *secret);

typedef NS_ENUM(NSUInteger, SKScreenIdiom) {
    SKScreenIdiomiPhone4 = 1,
    SKScreenIdiomiPhone5 = 0,
    SKScreenIdiomiPhone6 = 2,
    SKScreenIdiomiPhone6Plus = 3
};

/** Used to restructure JSON or modify an error returned from nearly any API call before being passed back to the application code. */
@protocol SKMiddleMan <NSObject>
/** @discussion This method is passed the JSON and any error generated by \c -[SKClient handleError:data:response:completion:].
 It is recommended that you pass the same error to the completion block as you were given in the first place, unless you work around it somehow.
 @param json The JSON from an API call.
 @param error The error generated by \c -[SKClient handleError:data:response:completion:].
 @param response The request response. Contains useful data like which endpoint the request was made on.
 @param completion The block to execute when you finish whatever you need to do. */
- (void)restructureJSON:(NSDictionary *)json error:(NSError *)error response:(NSURLResponse *)response completion:(ResponseBlock)completion;
@end

@interface SKClient : NSObject

/** The default Snapchat session manager. To use more than one account, simply create and manage your own instances of \c SKClient instead of using the singleton. */
+ (instancetype)sharedClient;

/** Initializes an \c SKClient instance with the minimum data required to resume an existing session. \c currentSession needs to be updated afterwards. */
+ (instancetype)clientWithUsername:(NSString *)username authToken:(NSString *)authToken gauth:(NSString *)googleAuthToken;

/** See the \c SKMiddleMan protocol. */
@property (nonatomic) id<SKMiddleMan> middleMan;

/** The size of your device's screen. On iOS, this defaults to the actual screen size. */
@property (nonatomic) CGSize screenSize;
/** The maxium sized to load videos in. On iOS, this defaults to the actual screen size. */
@property (nonatomic) CGSize maxVideoSize;

/** Use this to automatically adjust the \c screenSize and \c maxVideoSize properties. */
- (void)setScreenIdiom:(SKScreenIdiom)idiom;

/** The username of the currently signed in (or not yet singed in) user. @note Always lowercase. */
@property (nonatomic, readonly) NSString *username;
/** The \c SKSession object representing the current Snapchat session.
 @discussion Many of the categories on \c SKClient will automatically update this property; rarely is it necessary to attempt to update it yourself. */
@property (nonatomic) SKSession *currentSession;

// Data used to sign in

/** Used internally to sign in. Passed in either of the \c -restoreSessionWithUsername:snapchatAuthToken: methods as the \c snapchatAuthToken: parameter. */
@property (nonatomic, readonly) NSString *authToken;
/** Used internally to sign in. Passed as the \c googleAuthToken: parameter to \c -restoreSessionWithUsername:snapchatAuthToken:googleAuthToken: method. */
@property (nonatomic, readonly) NSString *googleAuthToken;
/** Used internally. */
@property (nonatomic, readonly) NSString *deviceToken1i;
/** Used internally. */
@property (nonatomic, readonly) NSString *deviceToken1v;
/** Used internally to sign in and trick Snapchat into thinking we're using the first party client. */
@property (nonatomic, readonly) NSString *googleAttestation;

/** Required to sign in properly. See https://clients.casper.io to get your own. */
@property (nonatomic) NSString *casperAPIKey;
/** Required to sign in properly. See https://clients.casper.io to get your own. */
@property (nonatomic) NSString *casperAPISecret;


#pragma mark Signing in
/** Signs into Snapchat.
 @discussion A valid GMail account is necessary to trick Snapchat into thinking we're using the first party client. Your data is only ever sent to Google, Scout's honor.
 @param username The Snapchat username to sign in with.
 @param password The password to the Snapchat account to sign in with.
 @param gmailEmail A vaild GMail address.
 @param gmailPassword The password associated with \c gmailEmail.
 @param completion Takes an error, if any, and the JSON response from signing in as a dictionary. */
- (void)signInWithUsername:(NSString *)username password:(NSString *)password gmail:(NSString *)gmailEmail gpass:(NSString *)gmailPassword completion:(DictionaryBlock)completion;
/** Signs into Snapchat without signing out an existing session by using that session's Snapchat auth token.
 @param username The Snapchat username to sign in with.
 @param authToken The auth token of an active Snapchat session.
 @param gmailEmail A vaild GMail address.
 @param gmailPassword The password associated with \c gmailEmail.
 @param completion Takes an error, if any, and the JSON response from signing in as a dictionary. */
- (void)signInWithUsername:(NSString *)username authToken:(NSString *)authToken gmail:(NSString *)gmailEmail gpass:(NSString *)gmailPassword completion:(DictionaryBlock)completion;
/** Use this to restore a session that ended within the last hour. The google auth token must be re-generated every hour.
 @discussion If you have a stale Google auth token, consider using \c -restoreSessionWithUsername:snapchatAuthToken:doGetUpdates:.
 @param username Your Snapchat username.
 @param authToken Your Snapchat auth token. Can be retrieved from the \c authToken property.
 @param gogoleAuthToken Your Google auth token. Can be retrieved from the \c googleAuthToken property.
 @param completion Takes an error, if any. */
- (void)restoreSessionWithUsername:(NSString *)username snapchatAuthToken:(NSString *)authToken googleAuthToken:(NSString *)googleAuthToken doGetUpdates:(ErrorBlock)completion;
/**  Signs out.
 @param completion Takes an error, if any. */
- (void)signOut:(ErrorBlock)completion;
/** Tells you if you're signed in or not. */
- (BOOL)isSignedIn;

#pragma mark Misc
/** Updates all information in the \c currentSession property.
 @param completion Takes an error, if any. */
- (void)updateSession:(ErrorBlock)completion;

#pragma mark Registration
/**
The first step in creating a new Snapchat account. Registers an email, password, and birthday in preparation for creating a new account.
 
 The dictionary passed to completion has the following keys:
 
    - \c email:                 the email you registered with.
 
    - \c snapchat_phone_number: a number you can use to verify your phone number later.
 
    - \c username_suggestions:  an array of available usernames for the next step.
 
 @param email The email address to be associated with the account.
 @param password The password of the account to be created.
 @param birthday Your birthday, in the format YYYY-MM-DD.
 @param completion Takes an error, if any, and a dictionary with some useful information. */
- (void)registerEmail:(NSString *)email password:(NSString *)password birthday:(NSString *)birthday completion:(DictionaryBlock)completion;
/**
 The second step in creating a new Snapchat account. Registers a username with an email that was registered in the first step.
 You must call this method after successfully completing the first step in registration.
 
 @param username The username of the account to be created, trimmed to the first 15 characters.
 @param registeredEmail The email address to be associated with the account, used in the first step of registration.
 @param gmail A valid GMail address. Required to make Snapchat think this is an official client.
 @param gpass The password to the Google account associated with gmail.
 @param completion Takes an error, if any. */
- (void)registerUsername:(NSString *)username withEmail:(NSString *)registeredEmail gmail:(NSString *)gmail gmailPassword:(NSString *)gpass completion:(ErrorBlock)completion;

// TODO: document response
/**
 The third and final step in registration. If you don't want to verify your humanity a phone number, you can verify it by with a "captcha" image of sorts.
 
 @param mobile A 10-digit (+ optional country code, defaults to 1) mobile phone number to be associated with the account, in any format. i.e. +11234567890, (123) 456-7890, 1-1234567890
 @param sms YES if you want a code sent via SMS, NO if you want to be called for verification.
 @param completion Takes an error, if any, and an undocumented JSON response.
 */
- (void)sendPhoneVerification:(NSString *)mobile sendText:(BOOL)sms completion:(DictionaryBlock)completion;
// TODO: document response, get completion working
/** Verifies your phone number, completing registration.
 @warning completion is not called in this implementaiton because I haven't tested it yet.
 @param code The code sent to verify your number.
 @param completion Takes an error, if any. */
- (void)verifyPhoneNumberWithCode:(NSString *)code completion:(ErrorBlock)completion;
/** Downloads captcha images to verify a new account with.
 @param completion Takes an error, if any, and an array of 9 \c NSData objects. */
- (void)getCaptcha:(ArrayBlock)completion;
// TODO: document response
/** Use this to "solve" a captcha. @warning Seems to not be working.
 @param solution The solution to the captcha as a binary string. If the first, second, and last images contain ghosts, the solution would be \c \@"110000001".
 @param completion Takes an error, if any, and an undocumented response. */
- (void)solveCaptchaWithSolution:(NSString *)solution completion:(DictionaryBlock)completion;

#pragma mark Internal
- (void)postTo:(NSString *)endpoint query:(NSDictionary *)query callback:(ResponseBlock)callback;
- (void)get:(NSString *)endpoint callback:(ResponseBlock)callback;
- (void)sendEvents:(NSArray *)events data:(NSDictionary *)snapInfo completion:(ErrorBlock)completion;
/** Completion is GUARANTEED to have one and only one non-nil parameter. */
- (void)handleError:(NSError *)error data:(NSData *)data response:(NSURLResponse *)response completion:(ResponseBlock)completion;

@end

/** Used to assert that we are signed in before making certain requests. */
NS_INLINE void SKAssertIsSignedIn() {
    if (![SKClient sharedClient].isSignedIn)
        [NSException raise:NSInternalInconsistencyException format:@"You must be signed in to call this method."];
}

/** Used to determine if the current device has an active connection. */
extern BOOL SKHasActiveConnection();