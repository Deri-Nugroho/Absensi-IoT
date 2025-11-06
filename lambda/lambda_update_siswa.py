import json
import boto3

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('BukuAbsenTable')

def handler(event, context):
    siswa_id = event['pathParameters']['id']
    body = json.loads(event['body'])
    nama = body.get('nama')
    pesan = body.get('pesan')

    update_expr = []
    expr_values = {}

    if nama:
        update_expr.append("nama = :n")
        expr_values[":n"] = nama
    if pesan:
        update_expr.append("pesan = :p")
        expr_values[":p"] = pesan

    if not update_expr:
        return {
            'statusCode': 400,
            'body': json.dumps({'error': 'Tidak ada data untuk diperbarui'})
        }

    table.update_item(
        Key={'id': siswa_id},
        UpdateExpression="SET " + ", ".join(update_expr),
        ExpressionAttributeValues=expr_values
    )

    return {
        'statusCode': 200,
        'headers': {'Access-Control-Allow-Origin': '*'},
        'body': json.dumps({'message': 'Data berhasil diperbarui'})
    }
