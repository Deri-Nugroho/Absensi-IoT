import json
import boto3

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('BukuAbsenTable')

def handler(event, context):
    siswa_id = event['pathParameters']['id']

    table.delete_item(Key={'id': siswa_id})
    return {
        'statusCode': 200,
        'headers': {'Access-Control-Allow-Origin': '*'},
        'body': json.dumps({'message': 'Data berhasil dihapus'})
    }
