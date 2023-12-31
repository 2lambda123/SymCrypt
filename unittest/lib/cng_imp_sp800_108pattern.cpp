//
// Pattern file for the CNG Sp800_108 implementations.
//
// Copyright (c) Microsoft Corporation. Licensed under the MIT license. 
//

VOID
KdfImp<ImpXxx,AlgSp800_108,BaseAlgXxx>::derive(
        _In_reads_( cbKey )     PCBYTE          pbKey,
                                SIZE_T          cbKey,
        _In_                    PKDF_ARGUMENTS  pArgs,
        _Out_writes_( cbDst )   PBYTE           pbDst, 
                                SIZE_T          cbDst )
{
    BCRYPT_KEY_HANDLE hKey;

    BCryptBuffer        buffer[5];
    BCryptBufferDesc    bufferDesc;

    CHECK3( NT_SUCCESS( CngGenerateSymmetricKeyFn( state.hAlg, &hKey, NULL, 0, (PBYTE) pbKey, (ULONG) cbKey, 0 ) ), "Could not generate key for Sp800_108-%s", STRING( BASE_Alg ) );

    bufferDesc.ulVersion = BCRYPTBUFFER_VERSION;
    bufferDesc.cBuffers = 0;
    bufferDesc.pBuffers = &buffer[0];

    AddBCryptBuffer( &bufferDesc, KDF_HASH_ALGORITHM, CNG_XXX_HASH_ALG_NAMEU, 2*wcslen( CNG_XXX_HASH_ALG_NAMEU ) );

    switch( pArgs->argType )
    {
    case KdfArgumentGeneric:
        AddBCryptBuffer( &bufferDesc, KDF_GENERIC_PARAMETER, pArgs->uGeneric.pbSelector, pArgs->uGeneric.cbSelector );
        break;
    case KdfArgumentSp800_108:
        AddBCryptBuffer( &bufferDesc, KDF_LABEL, pArgs->uSp800_108.pbLabel, pArgs->uSp800_108.cbLabel );
        AddBCryptBuffer( &bufferDesc, KDF_CONTEXT, pArgs->uSp800_108.pbContext, pArgs->uSp800_108.cbContext );
        break;
    default:
        CHECK( FALSE, "?" );
        return;
    }

    ULONG cbResult;
    CHECK( NT_SUCCESS( (*CngKeyDerivationFn)( hKey, &bufferDesc, pbDst, (ULONG) cbDst, &cbResult, 0 ) ), "Failure in CNG Sp800_108 call" );
    CHECK( cbResult == cbDst, "Sp800_108 result size mismatch" );
}

VOID
algImpDataPerfFunction<ImpXxx, AlgXxx, BaseAlgXxx>( PBYTE buf1, PBYTE buf2, PBYTE buf3, SIZE_T dataSize )
{
    BCryptBuffer        buffer[5];
    BCryptBufferDesc    bufferDesc;
    ULONG               cbResult;

    BCRYPT_KEY_HANDLE hKey = *(BCRYPT_KEY_HANDLE *) buf1;

    bufferDesc.ulVersion = BCRYPTBUFFER_VERSION;
    bufferDesc.cBuffers = 0;
    bufferDesc.pBuffers = &buffer[0];

    AddBCryptBuffer( &bufferDesc, KDF_HASH_ALGORITHM, CNG_XXX_HASH_ALG_NAMEU, 2*wcslen( CNG_XXX_HASH_ALG_NAMEU ) );
    AddBCryptBuffer( &bufferDesc, KDF_GENERIC_PARAMETER, buf2, 32 );

    (*CngKeyDerivationFn)( hKey, &bufferDesc, buf3, (ULONG)dataSize, &cbResult, 0 );
}
